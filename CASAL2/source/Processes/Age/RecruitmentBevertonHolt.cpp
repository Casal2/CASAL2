/**
 * @file RecruitmentBevertonHolt.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 12/07/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "RecruitmentBevertonHolt.h"

#include <limits>
#include <numeric>

#include "Categories/Categories.h"
#include "DerivedQuantities/Manager.h"
#include "Estimates/Manager.h"
#include "InitialisationPhases/Manager.h"
#include "TimeSteps/Manager.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"
#include "Utilities/Vector.h"

// namespaces
namespace niwa {
namespace processes {
namespace age {

namespace math = niwa::utilities::math;

/**
 * Default constructor
 */
RecruitmentBevertonHolt::RecruitmentBevertonHolt(shared_ptr<Model> model) : Process(model), partition_(model) {
  LOG_TRACE();

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The category labels", "");
  parameters_.Bind<Double>(PARAM_R0, &r0_, "R0, the mean recruitment used to scale annual recruits or initialise the model", "", false)->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_B0, &b0_, "B0, the SSB corresponding to R0, and used to scale annual recruits or initialise the model", "", false)->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_PROPORTIONS, &proportions_, "The proportion for each category", "");
  parameters_.Bind<unsigned>(PARAM_AGE, &age_, "The age at recruitment", "", true);
  parameters_.Bind<unsigned>(PARAM_SSB_OFFSET, &ssb_offset_, "The spawning biomass year offset", "", true);
  parameters_.Bind<Double>(PARAM_STEEPNESS, &steepness_, "Steepness (h)", "", 1.0)->set_range(0.2, 1.0);
  parameters_.Bind<string>(PARAM_SSB, &ssb_label_, "The SSB label (i.e., the derived quantity label)", "");
  parameters_.Bind<string>(PARAM_B0_PHASE, &phase_b0_label_, "The initialisation phase label that B0 is from", "", "");

  parameters_.Bind<Double>(PARAM_RECRUITMENT_MULTIPLIERS, &recruitment_multipliers_, "The YCS values", "");
  parameters_.Bind<unsigned>(PARAM_STANDARDISE_YEARS, &standardise_years_, "The years that are included for year class standardisation", "", true);

  // these inputs are deprecated
  parameters_.Bind<unsigned>(PARAM_STANDARDISE_YCS_YEARS, &standardise_ycs_years_, "The years that are included for year class standardisation", "", true);
  parameters_.Bind<Double>(PARAM_YCS_VALUES, &ycs_values_, "The YCS values", "", true);
  parameters_.Bind<unsigned>(PARAM_YCS_YEARS, &ycs_years_, "The recruitment years. A vector of years that relates to the year of the spawning event that created this cohort", "",
                             true);

  RegisterAsAddressable(PARAM_R0, &r0_);
  RegisterAsAddressable(PARAM_B0, &b0_);
  RegisterAsAddressable(PARAM_STEEPNESS, &steepness_);
  RegisterAsAddressable(PARAM_PROPORTIONS, &proportions_by_category_);
  RegisterAsAddressable(PARAM_RECRUITMENT_MULTIPLIERS, &recruitment_multipliers_by_year_);

  // Allow these to be used in additional priors.
  RegisterAsAddressable(PARAM_STANDARDISED_RECRUITMENT_MULTIPLIERS, &standardised_recruitment_multipliers_by_year_, addressable::kAll);

  phase_b0_            = 0;
  process_type_        = ProcessType::kRecruitment;
  partition_structure_ = PartitionType::kAge;
}

/**
 * Validate the process
 */
void RecruitmentBevertonHolt::DoValidate() {
  LOG_TRACE();
  // Flag error
  if (parameters_.Get(PARAM_YCS_VALUES)->has_been_defined())
    LOG_FATAL_P(PARAM_YCS_VALUES) << PARAM_YCS_VALUES << " is deprecated. The replacement subcommand is " << PARAM_RECRUITMENT_MULTIPLIERS
                                  << ". Refer to the user manual for more information";
  if (parameters_.Get(PARAM_YCS_YEARS)->has_been_defined())
    LOG_FATAL_P(PARAM_YCS_YEARS) << PARAM_YCS_YEARS << " is deprecated. Refer to user manual for more information";
  if (parameters_.Get(PARAM_STANDARDISE_YCS_YEARS)->has_been_defined())
    LOG_FATAL_P(PARAM_STANDARDISE_YCS_YEARS)
        << PARAM_STANDARDISE_YCS_YEARS << " is deprecated. Please use " << PARAM_STANDARDISE_YEARS
        << " to standardise. Note the years now refer to model years rather than the previous year_class_years. Refer to the user manual for more information";

  if (!parameters_.Get(PARAM_AGE)->has_been_defined()) {
    age_ = model_->min_age();
  } else if (age_ != model_->min_age()) {
    LOG_WARNING_P(PARAM_AGE) << "(" << age_ << ") is not equal to the model min_age (" << model_->min_age()
                             << "). This is likely an error. Please check your input configuration files";
  }

  if (parameters_.Get(PARAM_R0)->has_been_defined() && parameters_.Get(PARAM_B0)->has_been_defined())
    LOG_ERROR_P(PARAM_R0) << "Cannot specify both R0 and B0 in the same recruitment process.";

  if (!parameters_.Get(PARAM_R0)->has_been_defined() && !parameters_.Get(PARAM_B0)->has_been_defined())
    LOG_FATAL() << "Specify either R0 or B0 to initialise the model with Beverton-Holt recruitment";

  if (age_ < model_->min_age())
    LOG_ERROR_P(PARAM_AGE) << " (" << age_ << ") cannot be less than the model's min_age (" << model_->min_age() << ")";
  if (age_ > model_->max_age())
    LOG_ERROR_P(PARAM_AGE) << " (" << age_ << ") cannot be greater than the model's max_age (" << model_->max_age() << ")";

  if (category_labels_.size() != proportions_.size())
    LOG_ERROR_P(PARAM_CATEGORIES) << "One proportion is required to be defined per category. There are " << category_labels_.size() << " categories and " << proportions_.size()
                                  << " proportions defined.";

  Double running_total = 0.0;
  for (Double value : proportions_)  // Again, ADOLC prevents std::accum
    running_total += value;
  if (!utilities::math::IsOne(running_total))
    LOG_ERROR_P(PARAM_PROPORTIONS) << "The sum total is " << running_total << " which should be 1.0";

  for (auto year = model_->start_year(); year <= model_->final_year(); ++year) years_.push_back(year);

  if (recruitment_multipliers_.size() == 1) {
    Double temp = recruitment_multipliers_[0];
    recruitment_multipliers_.resize(years_.size(), temp);
  }


  if (recruitment_multipliers_.size() != years_.size()) {
    LOG_FATAL_P(PARAM_RECRUITMENT_MULTIPLIERS) << "There are " << years_.size() << " model years and " << recruitment_multipliers_.size() << " " << PARAM_RECRUITMENT_MULTIPLIERS
                                               << " defined. These inputs must be of equal length.";
  }
  // initialise ycs_values and check values aren't < 0.0
  unsigned ycs_iter = 0;
  for (unsigned ycs_year : years_) {
    recruitment_multipliers_by_year_[ycs_year]              = recruitment_multipliers_[ycs_iter];
    standardised_recruitment_multipliers_by_year_[ycs_year] = recruitment_multipliers_[ycs_iter];
    if (recruitment_multipliers_[ycs_iter] < 0.0)
      LOG_ERROR_P(PARAM_RECRUITMENT_MULTIPLIERS) << " value " << recruitment_multipliers_[ycs_iter] << " cannot be less than 0.0";
    ycs_iter++;
  }

  // Check ascending order
  if (standardise_years_.size() == 0) {
    standardise_recruitment_multipliers_ = false;
  } else if (standardise_years_.size() > 1) {
    for (unsigned i = 1; i < standardise_years_.size(); ++i) {
      LOG_FINE() << "standardised year = " << standardise_years_[i];
      if (standardise_years_[i] < model_->start_year())
        LOG_ERROR_P(PARAM_STANDARDISE_YEARS) << "cannot be less than model start year.";
      if (standardise_years_[i] > model_->final_year())
        LOG_ERROR_P(PARAM_STANDARDISE_YEARS) << "cannot be greater than model final year.";

      if (standardise_years_[i - 1] >= standardise_years_[i])
        LOG_ERROR_P(PARAM_STANDARDISE_YEARS) << "values must be in strictly increasing order. Value " << standardise_years_[i - 1] << " is not less than " << standardise_years_[i];
    }
    // need to focus on first value
    if (standardise_years_[0] < model_->start_year())
      LOG_ERROR_P(PARAM_STANDARDISE_YEARS) << "cannot be less than model start year.";
    if (standardise_years_[0] > model_->final_year())
      LOG_ERROR_P(PARAM_STANDARDISE_YEARS) << "cannot be greater than model final year.";
  }

  // Populate the proportions category, assumes there is a one to one relationship between categories, and proportions.
  unsigned iter = 0;
  for (auto& category : category_labels_) {
    proportions_by_category_[category] = proportions_[iter];
    ++iter;
  }
}

/**
 * Build the runtime relationships between this object and other objects
 */
void RecruitmentBevertonHolt::DoBuild() {
  partition_.Init(category_labels_);

  if (parameters_.Get(PARAM_B0)->has_been_defined()) {
    model_->set_b0_initialised(ssb_label_, true);
    b0_initialised_ = true;
  }
  if (phase_b0_label_ != "")
    phase_b0_ = model_->managers()->initialisation_phase()->GetPhaseIndex(phase_b0_label_);

  derived_quantity_ = model_->managers()->derived_quantity()->GetDerivedQuantity(ssb_label_);
  if (!derived_quantity_)
    LOG_ERROR_P(PARAM_SSB) << "Derived quantity SSB (" << ssb_label_ << ") was not found.";

  /**
   * Calculate out SSB offset
   */
  unsigned                temp_ssb_offset                  = 0;
  const vector<TimeStep*> ordered_time_steps               = model_->managers()->time_step()->ordered_time_steps();
  unsigned                time_step_index                  = 0;
  unsigned                process_index                    = 0;
  unsigned                ageing_processes                 = 0;
  unsigned                ageing_index                     = std::numeric_limits<unsigned>::max();
  unsigned                recruitment_index                = std::numeric_limits<unsigned>::max();
  unsigned                derived_quantity_index           = std::numeric_limits<unsigned>::max();
  unsigned                derived_quantity_time_step_index = model_->managers()->time_step()->GetTimeStepIndex(derived_quantity_->time_step());
  bool                    mortality_block                  = false;

  // loop through time steps
  for (auto time_step : ordered_time_steps) {
    if (time_step_index == derived_quantity_time_step_index) {
      for (auto process : time_step->processes()) {
        if (process->process_type() == ProcessType::kAgeing) {
          ageing_index = process_index;
          ageing_processes++;
        }
        if (process->process_type() == ProcessType::kMortality) {
          mortality_block        = true;
          derived_quantity_index = process_index;
        }
        process_index++;
      }
      LOG_FINEST() << "process_index = " << process_index;
      if (!mortality_block) {
        process_index++;
        derived_quantity_index = process_index;
        process_index++;
      }
    } else {
      for (auto process : time_step->processes()) {
        if (process->process_type() == ProcessType::kAgeing) {
          ageing_index = process_index;
          ageing_processes++;
        }
        process_index++;
      }
    }
    time_step_index++;
  }

  recruitment_index = model_->managers()->time_step()->GetProcessIndex(label_);
  if ((ageing_processes > 1) & !parameters_.Get(PARAM_SSB_OFFSET)->has_been_defined()) {
    LOG_ERROR_P(PARAM_LABEL) << "For the Beverton-Holt recruitment process, " << PARAM_SSB_OFFSET
                             << " can only be derived when there is only one ageing process in the annual cycle. " << ageing_processes
                             << " ageing processes were specified. Manually set the " << PARAM_SSB_OFFSET;
  }

  if (ageing_processes == 1) {
    if (ageing_index == std::numeric_limits<unsigned>::max())
      LOG_ERROR() << location() << " could not calculate the " << PARAM_SSB_OFFSET << " because there is no ageing process";

    if ((recruitment_index < ageing_index) && (ageing_index < derived_quantity_index))
      temp_ssb_offset = age_ + 1;
    else if (derived_quantity_index < ageing_index && ageing_index < recruitment_index)
      temp_ssb_offset = age_ - 1;
    else
      temp_ssb_offset = age_;

    LOG_FINEST() << PARAM_SSB_OFFSET << " calculated to be = " << temp_ssb_offset << "; recruitment index = " << recruitment_index << "; ageing index = " << ageing_index
                 << "; derived_quantity index = " << derived_quantity_index;
    // Check if the user has supplied the expected value for the model.
    if (parameters_.Get(PARAM_SSB_OFFSET)->has_been_defined()) {
      if (temp_ssb_offset != ssb_offset_) {
        LOG_WARNING() << "The " << PARAM_SSB_OFFSET << " specified (" << ssb_offset_ << ") is different from what Casal2 calculated (" << temp_ssb_offset
                      << "). This value should be manually set only under certain conditions. See the User Manual on this process for more information.";
      }
    } else {
      ssb_offset_ = temp_ssb_offset;
    }
  }
  spawn_event_years_.resize(model_->years().size(), 0.0);
  for (unsigned year_iter = 0; year_iter < model_->years().size(); ++year_iter) {
    spawn_event_years_[year_iter] = model_->years()[year_iter] - ssb_offset_;
    LOG_FINEST() << "ssb year = " << spawn_event_years_[year_iter] << " for year = " << model_->years()[year_iter];
  }

  // Check users haven't specified an @estimate block for both R0 and B0
  if (IsAddressableUsedFor(PARAM_R0, addressable::kEstimate) & IsAddressableUsedFor(PARAM_B0, addressable::kEstimate))
    LOG_ERROR() << "Both R0 and B0 have an @estimate specified for recruitment process " << label_ << ". Only one of these parameters can be estimated.";

  // Pre allocate memory
  ssb_values_.resize(model_->years().size());
  true_ycs_values_.resize(model_->years().size());
  recruitment_values_.resize(model_->years().size());

  DoReset();
}

/**
 * Verify all of the values so they are ready for an execution run
 */
void RecruitmentBevertonHolt::DoVerify(shared_ptr<Model> model) {
  // Check if PARAM_RECRUITMENT_MULTIPLIERS is used
  LOG_FINE() << "check transform usage = " << IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kTransformation);
  LOG_FINE() << "check lookup usage = " << IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kLookup);

  // if (!IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kTransformation)) {
  //    if (!IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kLookup))
  //     LOG_WARNING() << "The parameter " << PARAM_RECRUITMENT_MULTIPLIERS << " has no @additional_prior for it. It is recommended to have a vector_average additional prior";
  if (IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kTransformation)) {
    // Check if PARAM_RECRUITMENT_MULTIPLIERS has a transformation if it does don't let users
    // standardise.
    if (parameters_.Get(PARAM_STANDARDISE_YEARS)->has_been_defined())
      LOG_VERIFY() << "There is an @parameter_transformation for the parameter " << PARAM_RECRUITMENT_MULTIPLIERS
                   << ". If this is type=simplex, you should not specify the subcommand " << PARAM_STANDARDISE_YEARS;
  }

  if (model_->run_mode() == RunMode::kProjection) {
    if (IsAddressableUsedFor(PARAM_STANDARDISED_RECRUITMENT_MULTIPLIERS, addressable::kProject) & IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kProject))
      LOG_ERROR_P(PARAM_LABEL) << "- found an @project for both " << PARAM_STANDARDISED_RECRUITMENT_MULTIPLIERS << " and " << PARAM_RECRUITMENT_MULTIPLIERS
                               << ". This is not allowed, you must choose one or the other, but not both.";
    if (IsAddressableUsedFor(PARAM_STANDARDISED_RECRUITMENT_MULTIPLIERS, addressable::kProject)) {
      LOG_FINE() << "Projecting standardised multipliers";
      project_standardised_ycs_ = true;
    } else if (IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kProject)) {
      LOG_FINE() << "Projecting unstandardised multipliers";
      project_standardised_ycs_ = false;
    } else {
      LOG_ERROR_P(PARAM_LABEL) << "- could not find an @project block for " << PARAM_RECRUITMENT_MULTIPLIERS << " or " << PARAM_STANDARDISED_RECRUITMENT_MULTIPLIERS;
    }
  }
}
/**
 * Reset all of the values so they are ready for an execution run
 */
void RecruitmentBevertonHolt::DoReset() {
  LOG_TRACE();

  if (parameters_.Get(PARAM_B0)->has_been_defined()) {
    have_scaled_partition = false;
  }

  // if a -i call is made then we need to repopulate the ycs_values vector for reporting.
  // This has to be done because the input parameter ycs_values and registered estimate parameter ycs_values_by_year
  // Are different
  for (unsigned i = 0; i < years_.size(); ++i) {
    recruitment_multipliers_[i]                              = recruitment_multipliers_by_year_[years_[i]];
    standardised_recruitment_multipliers_by_year_[years_[i]] = recruitment_multipliers_by_year_[years_[i]];
  }
  unsigned iter = 0;
  for (auto& category : category_labels_) {
    proportions_[iter] = proportions_by_category_[category];
    ++iter;
  }
  fill(ssb_values_.begin(), ssb_values_.end(), 0.0);
  fill(true_ycs_values_.begin(), true_ycs_values_.end(), 0.0);
  fill(recruitment_values_.begin(), recruitment_values_.end(), 0.0);

  // Do Haist ycs Parametrisation
  if (standardise_recruitment_multipliers_) {
    mean_ycs_ = 0;
    for (unsigned i = 0; i < years_.size(); ++i) {
      for (unsigned j = 0; j < standardise_years_.size(); ++j) {
        if (years_[i] == standardise_years_[j]) {
          mean_ycs_ += recruitment_multipliers_by_year_[years_[i]];
          break;
        }
      }
    }

    mean_ycs_ /= standardise_years_.size();
    for (unsigned ycs_year : years_) {
      for (unsigned j = 0; j < standardise_years_.size(); ++j) {
        if (ycs_year == standardise_years_[j])
          standardised_recruitment_multipliers_by_year_[ycs_year] = recruitment_multipliers_by_year_[ycs_year] / mean_ycs_;
      }
    }
  } else {
    for (unsigned ycs_year : years_) {
      standardised_recruitment_multipliers_by_year_[ycs_year] = recruitment_multipliers_by_year_[ycs_year];
    }
  }
}

/**
 * Execute this process
 */
void RecruitmentBevertonHolt::DoExecute() {
  unsigned             current_year   = model_->current_year();
  std::pair<bool, int> this_year_iter = niwa::utilities::findInVector(model_->years(), current_year);
  year_counter_                       = this_year_iter.second;
  unsigned ssb_year                   = current_year - ssb_offset_;
  LOG_FINE() << "year = " << current_year << " ssb year = " << ssb_year << " year counter = " << year_counter_;
  Double amount_per = 0.0;
  if (model_->state() == State::kInitialise) {
    initialisationphases::Manager& init_phase_manager = *model_->managers()->initialisation_phase();
    if ((init_phase_manager.last_executed_phase() <= phase_b0_) && (parameters_.Get(PARAM_R0)->has_been_defined())) {
      amount_per = r0_;
    } else if ((init_phase_manager.last_executed_phase() <= phase_b0_) && (parameters_.Get(PARAM_B0)->has_been_defined())) {
      if (have_scaled_partition)
        amount_per = r0_;
      else
        amount_per = 1;
    } else {
      // if R0 initialised mode then b0 is a derived quantity
      if (!parameters_.Get(PARAM_B0)->has_been_defined())
        b0_ = derived_quantity_->GetLastValueFromInitialisation(phase_b0_);

      Double SSB       = derived_quantity_->GetLastValueFromInitialisation(init_phase_manager.last_executed_phase());
      Double ssb_ratio = SSB / b0_;
      Double true_ycs  = 1.0 * ssb_ratio / (1.0 - ((5.0 * steepness_ - 1.0) / (4.0 * steepness_)) * (1.0 - ssb_ratio));
      amount_per       = r0_ * true_ycs;

      LOG_FINE() << "B0_: " << b0_ << "; ssb_ratio: " << ssb_ratio << "; true_ycs: " << true_ycs << "; amount_per: " << amount_per << " R0 = " << r0_;
    }
    LOG_FINE() << "Initialise: amount_per = " << amount_per;
  } else {
    /**
     * The model is not in an initialisation phase
     */
    LOG_FINEST() << "standardise_years_.size(): " << standardise_years_.size() << "; model_->current_year(): " << current_year
                 << "; model_->start_year(): " << model_->start_year();
    Double ycs;
    // If projection mode ycs values get replaced with projected value from @project block
    // note that the container recruitment_multipliers_by_year_ is changed by time_varying and projection classes
    // but the code wants to use standardised_recruitment_multipliers_by_year_ in the functions following here, so we might need to update this.
    if (model_->run_mode() == RunMode::kProjection) {
      if (recruitment_multipliers_by_year_[current_year] == 0.0) {
        LOG_FATAL_P(PARAM_RECRUITMENT_MULTIPLIERS) << "Projection mode (-f) is being run but ycs values are = 0 for year " << model_->current_year()
                                                   << ", which will cause the recruitment process to supply 0 recruits. Please check the @project block for this parameter";
      }
      // Projection classes will update this container automatically
      ycs = recruitment_multipliers_by_year_[current_year];
      // We need to see if this value has changed from the initial input, if it has we are going to assume that this is because the projection class has changed it.
      // set standardised ycs = ycs for reporting
      if (current_year > model_->final_year()) {
        // we are in projection years so force standardised ycs to be the same as recruitment_multipliers_by_year_[ssb_year];
        if (project_standardised_ycs_) {
          // reporting purpose
          recruitment_multipliers_by_year_[current_year] = standardised_recruitment_multipliers_by_year_[current_year];
          // Change ycs
          ycs = standardised_recruitment_multipliers_by_year_[current_year];
        } else {
          standardised_recruitment_multipliers_by_year_[current_year] = recruitment_multipliers_by_year_[current_year];
        }
      } else {
        // we are still within start-final_year need to check if we have overwritten any values.
        if (ycs != recruitment_multipliers_[year_counter_])
          standardised_recruitment_multipliers_by_year_[current_year] = ycs;
        else
          ycs = standardised_recruitment_multipliers_by_year_[current_year];
      }
      LOG_FINE() << "ssb year = " << ssb_year << " value = " << ycs << " last val = " << model_->final_year() << " counter = " << year_counter_ << " size of vector "
                 << recruitment_multipliers_.size();
      LOG_FINE() << "Projected ycs = " << ycs << " what is in the original " << recruitment_multipliers_[year_counter_];
      // else business as usual
    } else {
      ycs = standardised_recruitment_multipliers_by_year_[current_year];
      LOG_FINE() << "ycs" << ycs << " for SSB year " << ssb_year << " current year = " << current_year;
    }

    // Check whether B0 as an input paramter or a derived quantity, this is a result of having an r0 or a b0 in the process
    if (!parameters_.Get(PARAM_B0)->has_been_defined())
      b0_ = derived_quantity_->GetLastValueFromInitialisation(phase_b0_);

    // Calculate year to get SSB that contributes to this years recruits
    Double SSB;
    if (ssb_year < model_->start_year()) {
      // Model is in normal years but requires an SSB from the initialisation phase
      initialisationphases::Manager& init_phase_manager = *model_->managers()->initialisation_phase();
      LOG_FINE() << "Initialisation phase index SSB is being extracted from init phase " << init_phase_manager.last_executed_phase() << " SSB year = " << ssb_year;
      SSB = derived_quantity_->GetLastValueFromInitialisation(init_phase_manager.last_executed_phase());
    } else {
      // else get value from offset
      SSB = derived_quantity_->GetValue(ssb_year);
    }

    Double ssb_ratio = SSB / b0_;
    Double SR        = ssb_ratio / (1.0 - ((5.0 * steepness_ - 1.0) / (4.0 * steepness_)) * (1.0 - ssb_ratio));
    Double true_ycs  = ycs * SR;
    amount_per       = r0_ * true_ycs;

    true_ycs_values_[year_counter_]    = true_ycs;
    recruitment_values_[year_counter_] = amount_per;
    ssb_values_[year_counter_]         = SSB;

    LOG_FINEST() << "year = " << model_->current_year() << " SSB = " << SSB << " SR = " << SR << "; ycs = " << recruitment_multipliers_by_year_[current_year]
                 << " Standardised year class = " << standardised_recruitment_multipliers_by_year_[current_year] << "; B0_ = " << b0_ << "; R0 = " << r0_
                 << "; ssb_ratio = " << ssb_ratio << "; true_ycs = " << true_ycs << "; amount_per = " << amount_per;
  }

  unsigned i = 0;
  for (auto category : partition_) {
    LOG_FINEST() << category->name_ << "; age: " << age_ << "; category->min_age_: " << category->min_age_ << " recruits = " << amount_per << ", proportion of recruits "
                 << proportions_by_category_[category->name_];
    category->data_[age_ - category->min_age_] += amount_per * proportions_by_category_[category->name_];
    ++i;
  }
}

/**
 *  Called in the initialisation phase, this method scales the partition affected by this recruitment event if recruitment is B0 initialised
 */
void RecruitmentBevertonHolt::ScalePartition() {
  if (!parameters_.Get(PARAM_B0)->has_been_defined())
    LOG_CODE_ERROR() << "Cannot apply this method as B0 has not been defined";

  have_scaled_partition  = true;
  Double alternative_ssb = derived_quantity_->GetValue(model_->start_year() - ssb_offset_);

  // Look at initialisation phase
  Double SSB = derived_quantity_->GetLastValueFromInitialisation(phase_b0_);
  if (SSB <= 0.0)
    LOG_FATAL() << "SSB from initialisation was '" << SSB << "' which is invalid. Try doing a run with '--loglevel trace' to determine the error";
  LOG_FINEST() << "Last SSB value = " << SSB << " init ssb = " << alternative_ssb;
  Double scalar = b0_ / SSB;
  LOG_FINEST() << "Scalar = " << scalar << " B0 = " << b0_;
  LOG_FINEST() << "R0 = " << scalar;
  r0_ = scalar;
  for (auto category : partition_) {
    for (unsigned j = 0; j < category->data_.size(); ++j) {
      LOG_FINEST() << "Category " << category->name_ << " Age = " << j + category->min_age_ << " Numbers at age before = " << category->data_[j];
      category->data_[j] *= scalar;
      LOG_FINEST() << "Category " << category->name_ << " Age = " << j + category->min_age_ << " Numbers at age = " << category->data_[j];
    }
  }
  LOG_FINEST() << "R0 = " << r0_;
}

/**
 * Fill the report cache
 */
void RecruitmentBevertonHolt::FillReportCache(ostringstream& cache) {
  cache << "model_year: ";
  for (auto iter : standardised_recruitment_multipliers_by_year_) cache << iter.first << " ";
  cache << "\nspawn_event_year: ";
  for (auto iter : spawn_event_years_) cache << iter << " ";
  cache << "\nstandardised_recruitment_multipliers: ";
  for (auto iter : standardised_recruitment_multipliers_by_year_) cache << AS_DOUBLE(iter.second) << " ";
  cache << "\nrecruitment_multipliers: ";
  for (auto iter : recruitment_multipliers_by_year_) cache << AS_DOUBLE(iter.second) << " ";
  cache << "\ntrue_ycs: ";
  for (auto iter : true_ycs_values_) cache << AS_DOUBLE(iter) << " ";
  cache << "\nrecruits: ";
  for (auto iter : recruitment_values_) cache << AS_DOUBLE(iter) << " ";
  cache << "\nrecruit_event_SSB: ";
  for (auto iter : ssb_values_) cache << AS_DOUBLE(iter) << " ";
  cache << "\nrecruit_event_SSB_percent: ";
  for (auto iter : ssb_values_) cache << AS_DOUBLE(iter) / AS_DOUBLE(b0_) * 100.0 << " ";
  cache << "\nssb_offset: " << ssb_offset_;
  cache << REPORT_EOL;
}

/**
 * Fill the tabular report cache
 */
void RecruitmentBevertonHolt::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {
    vector<unsigned> years = model_->years();

    for (auto iter : standardised_recruitment_multipliers_by_year_) cache << "standardised_recruitment_multipliers[" << iter.first << "] ";
    for (auto iter : recruitment_multipliers_by_year_) cache << "recruitment_multipliers[" << iter.first << "] ";

    for (auto year : years) {
      unsigned ssb_year = year - ssb_offset_;
      cache << "true_ycs[" << ssb_year << "] ";
    }
    for (auto year : years) {
      cache << "recruits[" << year << "] ";
    }
    for (auto year : years) {
      unsigned ssb_year = year - ssb_offset_;
      cache << "recruit_event_SSB[" << ssb_year << "] ";
    }

    cache << "R0 B0 steepness SSB_offset ";
    cache << REPORT_EOL;
  }

  for (auto iter : standardised_recruitment_multipliers_by_year_) cache << AS_DOUBLE(iter.second) << " ";
  for (auto iter : recruitment_multipliers_by_year_) cache << AS_DOUBLE(iter.second) << " ";

  for (auto value : true_ycs_values_) cache << AS_DOUBLE(value) << " ";
  for (auto value : recruitment_values_) cache << AS_DOUBLE(value) << " ";
  for (auto value : ssb_values_) cache << AS_DOUBLE(value) << " ";

  cache << AS_DOUBLE(r0_) << " " << AS_DOUBLE(b0_) << " " << AS_DOUBLE(steepness_) << " " << ssb_offset_ << " ";
  cache << REPORT_EOL;
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
