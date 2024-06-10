/**
 * @file RecruitmentBevertonHoltWithDeviations.cpp
 * @author C.Marsh
 * @date 11/7/17
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "RecruitmentBevertonHoltWithDeviations.h"

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
RecruitmentBevertonHoltWithDeviations::RecruitmentBevertonHoltWithDeviations(shared_ptr<Model> model) : Process(model), partition_(model) {
  LOG_TRACE();

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The category labels", "");
  parameters_.Bind<Double>(PARAM_R0, &r0_, "R0, the mean recruitment used to scale annual recruits or initialise the model", "", false)->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_B0, &b0_, "B0, the SSB corresponding to R0, and used to scale annual recruits or initialise the model", "", false)->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_PROPORTIONS, &proportions_, "The proportion for each category", "");
  parameters_.Bind<unsigned>(PARAM_AGE, &age_, "The age at recruitment", "", true);
  parameters_.Bind<unsigned>(PARAM_SSB_OFFSET, &ssb_offset_, "The spawning biomass year offset", "", true);
  parameters_.Bind<Double>(PARAM_STEEPNESS, &steepness_, "Steepness (h)", "", 1.0)->set_range(0.2, 1.0);
  parameters_.Bind<string>(PARAM_SSB, &ssb_, "The SSB label (i.e., the derived quantity label))", "");
  parameters_.Bind<Double>(PARAM_SIGMA_R, &sigma_r_, "The standard deviation of recruitment, sigma_R", "")->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_B_MAX, &b_max_, "The maximum bias adjustment", "", 0.85)->set_range(0.0, 1.0);
  parameters_.Bind<unsigned>(PARAM_LAST_YEAR_WITH_NO_BIAS, &year1_, "The last year (recruited year) with no bias adjustment", "", false);
  parameters_.Bind<unsigned>(PARAM_FIRST_YEAR_WITH_BIAS, &year2_, "The first year (recruited year) with full bias adjustment", "", false);
  parameters_.Bind<unsigned>(PARAM_LAST_YEAR_WITH_BIAS, &year3_, "The last year (recruited year) with full bias adjustment", "", false);
  parameters_.Bind<unsigned>(PARAM_FIRST_RECENT_YEAR_WITH_NO_BIAS, &year4_, "The first recent year (recruited year) with no bias adjustment", "", false);

  parameters_.Bind<string>(PARAM_B0_PHASE, &phase_b0_label_, "The initialisation phase label that B0 is from", "", "");
  parameters_.Bind<Double>(PARAM_DEVIATION_VALUES, &recruit_dev_values_, "The recruitment deviation values", "");
  parameters_.Bind<unsigned>(PARAM_DEVIATION_YEARS, &recruit_dev_years_,
                             "The recruitment years. A vector of years that relates to the year of the spawning event that created this cohort", "", true);

  RegisterAsAddressable(PARAM_R0, &r0_);
  RegisterAsAddressable(PARAM_B0, &b0_);
  RegisterAsAddressable(PARAM_B_MAX, &b_max_);
  RegisterAsAddressable(PARAM_SIGMA_R, &sigma_r_);
  RegisterAsAddressable(PARAM_STEEPNESS, &steepness_);
  RegisterAsAddressable(PARAM_PROPORTIONS, &proportions_by_category_);
  RegisterAsAddressable(PARAM_DEVIATION_VALUES, &recruit_dev_value_by_year_);

  phase_b0_            = 0;
  process_type_        = ProcessType::kRecruitment;
  partition_structure_ = PartitionType::kAge;
}

/**
 * Validate
 */
void RecruitmentBevertonHoltWithDeviations::DoValidate() {
  LOG_TRACE();
  if (parameters_.Get(PARAM_DEVIATION_YEARS)->has_been_defined())
    LOG_ERROR_P(PARAM_DEVIATION_YEARS) << PARAM_DEVIATION_YEARS << " is deprecated, refer to user manual for more information.";

  recruit_dev_years_ = model_->years();  // set to model years

  if (!parameters_.Get(PARAM_AGE)->has_been_defined()) {
    age_ = model_->min_age();
  } else if (age_ != model_->min_age()) {
    LOG_WARNING_P(PARAM_AGE) << "(" << age_ << ") is not equal to the model min_age (" << model_->min_age()
                             << "). This is likely an error. Please check your input configuration files";
  }

  if (parameters_.Get(PARAM_R0)->has_been_defined() && parameters_.Get(PARAM_B0)->has_been_defined())
    LOG_FATAL_P(PARAM_R0) << "Cannot specify both R0 and B0 in the model for Beverton-Holt recruitment";

  if (!parameters_.Get(PARAM_R0)->has_been_defined() && !parameters_.Get(PARAM_B0)->has_been_defined())
    LOG_FATAL() << "Specify either R0 or B0 to initialise the model for Beverton-Holt recruitment";

  if (age_ < model_->min_age())
    LOG_ERROR_P(PARAM_AGE) << "(" << age_ << ") cannot be less than the model's min_age (" << model_->min_age() << ")";
  if (age_ > model_->max_age())
    LOG_ERROR_P(PARAM_AGE) << "(" << age_ << ") cannot be greater than the model's max_age (" << model_->max_age() << ")";

  if (category_labels_.size() != proportions_.size())
    LOG_ERROR_P(PARAM_CATEGORIES) << "One proportion is required to be defined per category. There are " << category_labels_.size() << " categories and " << proportions_.size()
                                  << " proportions defined.";

  Double running_total = 0.0;
  for (Double value : proportions_)  // Again, ADOLC prevents std::accum
    running_total += value;
  if (!math::IsOne(running_total))
    LOG_ERROR_P(PARAM_PROPORTIONS) << "The sum total is " << running_total << " which should be 1.0";

  for (auto year = model_->start_year(); year <= model_->final_year(); ++year) years_.push_back(year);

  if (recruit_dev_values_.size() != years_.size()) {
    LOG_ERROR_P(PARAM_DEVIATION_VALUES) << "There must be a recruitment deviation specified for all years between start and final. Expected " << years_.size() << ", but found "
                                        << recruit_dev_values_.size();
  }

  // initialise recruit devs
  unsigned ycs_iter = 0;
  for (unsigned recruited_year : years_) {
    recruit_dev_value_by_year_[recruited_year] = recruit_dev_values_[ycs_iter];
    ycs_iter++;
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
void RecruitmentBevertonHoltWithDeviations::DoBuild() {
  partition_.Init(category_labels_);

  if (parameters_.Get(PARAM_B0)->has_been_defined()) {
    model_->set_b0_initialised(ssb_, true);
    b0_initialised_ = true;
  }
  if (phase_b0_label_ != "")
    phase_b0_ = model_->managers()->initialisation_phase()->GetPhaseIndex(phase_b0_label_);

  derived_quantity_ = model_->managers()->derived_quantity()->GetDerivedQuantity(ssb_);
  if (!derived_quantity_)
    LOG_ERROR_P(PARAM_SSB) << "Derived quantity SSB (" << ssb_ << ") was not found.";

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
        LOG_FINEST() << "Entering this loop?" << process_index;
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
  if (ageing_processes > 1)
    LOG_ERROR_P(PARAM_SSB_OFFSET) << "The Beverton-Holt recruitment year offset has been calculated on the basis of a single ageing process. " << ageing_processes
                                  << " ageing processes were specified. Manually set the ssb_offset or contact the development team";
  if (ageing_index == std::numeric_limits<unsigned>::max())
    LOG_ERROR() << location() << " could not calculate the ssb_offset because there is no ageing process defined";

  if (recruitment_index < ageing_index && ageing_index < derived_quantity_index)
    temp_ssb_offset = age_ + 1;
  else if (derived_quantity_index < ageing_index && ageing_index < recruitment_index)
    temp_ssb_offset = age_ - 1;
  else
    temp_ssb_offset = age_;

  LOG_FINEST() << "SSB offset calculated to be = " << temp_ssb_offset << "; recruitment index = " << recruitment_index << "; ageing index = " << ageing_index
               << "; derived_quantity index = " << derived_quantity_index;
  if (parameters_.Get(PARAM_SSB_OFFSET)->has_been_defined()) {
    // Check if the user has supplied the expected value for the model.
    if (temp_ssb_offset != ssb_offset_) {
      LOG_WARNING() << "The " << PARAM_SSB_OFFSET << " specified (" << ssb_offset_ << ") is different from what Casal2 calculated (" << temp_ssb_offset
                    << "). This value should be manually set only under certain conditions. See the User Manual on this process for more information.";
    }
  } else {
    ssb_offset_ = temp_ssb_offset;
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
  // Check if recruitment devs have an @estimate block, I am just checking over the first year
  for (auto year : years_) {
    string year_string;
    if (!utilities::To<unsigned, string>(year, year_string))
      LOG_CODE_ERROR() << "Could not convert the value " << year << " to a string";

    string recruit_parm = "process[" + label_ + "]." + PARAM_DEVIATION_VALUES + "{" + year_string + "}";
    if (model_->managers()->estimate()->HasEstimate(recruit_parm)) {
      Estimate* recruit_dev_estimate = model_->managers()->estimate()->GetEstimate(recruit_parm);
      if (!recruit_dev_estimate)
        LOG_CODE_ERROR() << "'!sigma_r_estimate_', parameter " << recruit_parm << " has estimate but the parameter cannot be estimated.";
      if (recruit_dev_estimate->type() != PARAM_NORMAL_BY_STDEV)
        LOG_ERROR() << "An @estimate block for " << recruit_parm << " is not of type " << PARAM_NORMAL_BY_STDEV << ". @estimate blocks only for deviation_values of type "
                    << PARAM_NORMAL_BY_STDEV << " are valid.";
      // Check sigma is the same as sigma_r
      map<string, Parameter*> parameters = recruit_dev_estimate->parameters().parameters();
      for (auto param = parameters.begin(); param != parameters.end(); ++param) {
        if (param->first == PARAM_SIGMA) {
          Double estimate_sigma = 0.0;
          if (!utilities::To<string, Double>(param->second->values()[0], estimate_sigma))
            LOG_CODE_ERROR() << "Could not convert the value " << param->second->values()[0] << " to a Double";

          if (fabs(estimate_sigma - sigma_r_) > 0.001) {
            LOG_FATAL_P(PARAM_SIGMA_R) << "@estimate block for parameter " << recruit_parm << " has sigma = " << estimate_sigma << ". sigma = " << sigma_r_
                                       << " is specified in this process. These two sigma values must be the same.";
          }
        }
      }
    }
  }

  // Build Bias correction map by year 'bias_by_year_'
  for (auto year : years_) {
    if (year <= year1_) {
      bias_by_year_[year] = 0.0;
    } else if ((year > year1_) && (year < year2_)) {
      bias_by_year_[year] = b_max_ * (Double)(1 - ((year - year1_) / (year2_ - year1_)));
    } else if ((year >= year2_) && (year <= year3_)) {
      bias_by_year_[year] = b_max_;
    } else if ((year > year3_) && (year < year4_)) {
      bias_by_year_[year] = b_max_ * (Double)(1 - ((year3_ - year) / (year4_ - year3_)));
    } else if (year >= year4_) {
      bias_by_year_[year] = 0.0;
    }
  }
  // Pre allocate memory for report objects
  ssb_values_.resize(model_->years().size());
  true_ycs_values_.resize(model_->years().size());
  recruitment_values_.resize(model_->years().size());
  ycs_values_.resize(model_->years().size());

  DoReset();
}

/**
 * Reset all of the values so they are ready for an execution run
 *
 * - check if is necessary to rescale the partition
 * - update input parameters to updated parameters
 * - clear reporting containers
 * - check where B0 is coming from
 */
void RecruitmentBevertonHoltWithDeviations::DoReset() {
  LOG_TRACE();
  if (parameters_.Get(PARAM_B0)->has_been_defined()) {
    have_scaled_partition = false;
  }
  // if a -i call is made then we need to re-populate the recruit_dev_values_ vector for reporting.
  // This has to be done because the input parameter recruit_dev_values_ and registered estimate parameter recruit_dev_value_by_year_
  // Are different
  for (unsigned i = 0; i < years_.size(); ++i) {
    recruit_dev_values_[i] = recruit_dev_value_by_year_[years_[i]];
  }

  // clear containers for another annual cycle.
  fill(ssb_values_.begin(), ssb_values_.end(), 0.0);
  fill(true_ycs_values_.begin(), true_ycs_values_.end(), 0.0);
  fill(recruitment_values_.begin(), recruitment_values_.end(), 0.0);
  fill(ycs_values_.begin(), ycs_values_.end(), 0.0);

  // Check whether B0 as an input paramter or a derived quantity, this is a result of having an r0 or a b0 in the process
  // if its estimated or an input it will be updates.
  if (!parameters_.Get(PARAM_B0)->has_been_defined())
    b0_ = derived_quantity_->GetLastValueFromInitialisation(phase_b0_);

  // Only rebuild in the reset if Bmax is estimated, otherwise it remains constant.
  if (model_->managers()->estimate()->HasEstimate("process[" + label_ + "].b_max")) {  // this check should be moved into the Build and set a flag
    // Build Bias correction map by year 'bias_by_year_'
    for (auto year : recruit_dev_years_) {
      if (year <= year1_) {
        bias_by_year_[year] = 0.0;
      } else if ((year > year1_) && (year < year2_)) {
        bias_by_year_[year] = b_max_ * (Double)(1 - ((year - year1_) / (year2_ - year1_)));
      } else if ((year >= year2_) && (year <= year3_)) {
        bias_by_year_[year] = b_max_;
      } else if ((year > year3_) && (year < year4_)) {
        bias_by_year_[year] = b_max_ * (Double)(1 - ((year3_ - year) / (year4_ - year3_)));
      } else if (year >= year4_) {
        bias_by_year_[year] = 0.0;
      }
    }
  }
  unsigned iter = 0;
  for (auto& category : category_labels_) {
    proportions_[iter] = proportions_by_category_[category];
    ++iter;
  }
}

/**
 * Execute this process.
 *
 */
void RecruitmentBevertonHoltWithDeviations::DoExecute() {
  unsigned             current_year   = model_->current_year();
  std::pair<bool, int> this_year_iter = niwa::utilities::findInVector(model_->years(), current_year);
  year_counter_                       = this_year_iter.second;
  unsigned ssb_year                   = current_year - ssb_offset_;

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
      // if R0 intialised mode then b0 is a derived quantity
      if (!parameters_.Get(PARAM_B0)->has_been_defined())
        b0_ = derived_quantity_->GetLastValueFromInitialisation(phase_b0_);

      Double SSB       = derived_quantity_->GetLastValueFromInitialisation(init_phase_manager.last_executed_phase());
      Double ssb_ratio = SSB / b0_;
      Double true_ycs  = 1.0 * ssb_ratio / (1 - ((5 * steepness_ - 1) / (4 * steepness_)) * (1 - ssb_ratio));
      amount_per       = r0_ * true_ycs;

      LOG_FINEST() << "b0_: " << b0_ << "; ssb_ratio: " << ssb_ratio << "; true_ycs: " << true_ycs << "; amount_per: " << amount_per;
    }

  } else {
    /**
     * The model is not in an initialisation phase
     */
    LOG_FINEST() << "; model_->start_year(): " << model_->start_year();
    Double ycs = 0.0;
    if (recruit_dev_value_by_year_.find(current_year) != recruit_dev_value_by_year_.end() && bias_by_year_.find(current_year) != bias_by_year_.end())
      ycs = exp(recruit_dev_value_by_year_[current_year] - (bias_by_year_[current_year] * 0.5 * sigma_r_ * sigma_r_));
    LOG_FINEST() << "Projected ycs = " << ycs;

    if (ycs < 0.0)
      LOG_CODE_ERROR() << "'ycs < 0.0' negative recruitment";

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
    ycs_values_[year_counter_]         = ycs;
    LOG_FINEST() << "year = " << model_->current_year() << " SSB = " << SSB << " SR = " << SR << "; recruit_dev = " << recruit_dev_value_by_year_[current_year] << "; b0_ = " << b0_
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
 *  Called in the intialisation phase, this method scales the partition affected by this recruitment event if recruitment is B0 initialised
 */
void RecruitmentBevertonHoltWithDeviations::ScalePartition() {
  if (!parameters_.Get(PARAM_B0)->has_been_defined())
    LOG_CODE_ERROR() << "Cannot apply this method as B0 has not been defined";

  have_scaled_partition = true;
  Double SSB            = derived_quantity_->GetValue(model_->start_year() - ssb_offset_);
  LOG_FINEST() << "Last SSB value = " << SSB;
  Double scalar = b0_ / SSB;
  LOG_FINEST() << "Scalar = " << scalar << " B0 = " << b0_;
  LOG_FINEST() << "R0 = " << scalar;
  r0_ = scalar;
  for (auto category : partition_) {
    for (unsigned j = 0; j < category->data_.size(); ++j) {
      LOG_FINEST() << "Category " << category->name_ << " Age = " << j + category->min_age_ << " Numbers at age = " << category->data_[j];
      category->data_[j] *= scalar;
      LOG_FINEST() << "Category " << category->name_ << " Age = " << j + category->min_age_ << " Numbers at age = " << category->data_[j];
    }
  }
  LOG_FINEST() << "R0 = " << r0_;
}

/**
 * Fill the report cache
 * @description A method for reporting process information
 * @param cache a cache object to print to
 */
void RecruitmentBevertonHoltWithDeviations::FillReportCache(ostringstream& cache) {
  cache << "recruitment_multipliers: ";
  for (auto iter : ycs_values_) cache << AS_DOUBLE(iter) << " ";
  cache << "\nmodel_year: ";
  for (auto iter : recruit_dev_value_by_year_) cache << iter.first << " ";
  cache << "\nspawn_event_year: ";
  for (auto iter : spawn_event_years_) cache << iter << " ";
  cache << "\ntrue_ycs: ";
  for (auto iter : true_ycs_values_) cache << AS_DOUBLE(iter) << " ";
  cache << "\nrecruits: ";
  for (auto iter : recruitment_values_) cache << AS_DOUBLE(iter) << " ";
  cache << "\nrecruit_event_SSB: ";
  for (auto iter : ssb_values_) cache << AS_DOUBLE(iter) << " ";
  cache << "\nrecruit_event_SSB_percent: ";
  for (auto iter : ssb_values_) cache << AS_DOUBLE(iter) / AS_DOUBLE(b0_) * 100.0 << " ";
  cache << REPORT_EOL;
}

/**
 * Fill the tabular report cache
 * @description A method for reporting tabular process information
 * @param cache a cache object to print to
 * @param first_run whether to print the header
 *
 */
void RecruitmentBevertonHoltWithDeviations::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {
    vector<unsigned> years = model_->years();
    for (auto year : years) {
      unsigned ssb_year = year - ssb_offset_;
      cache << "ycs_values[" << ssb_year << "] ";
    }
    for (auto year : years) {
      unsigned ssb_year = year - ssb_offset_;
      cache << "true_ycs[" << ssb_year << "] ";
    }
    for (auto year : years) {
      cache << "Recruits[" << year << "] ";
    }
    for (auto year : years) {
      unsigned ssb_year = year - ssb_offset_;
      cache << "Recruit_event_SSB[" << ssb_year << "] ";
    }
    cache << "R0 B0 steepness ";
    cache << REPORT_EOL;
  }

  for (auto value : ycs_values_) cache << AS_DOUBLE(value) << " ";
  for (auto value : true_ycs_values_) cache << AS_DOUBLE(value) << " ";
  for (auto value : recruitment_values_) cache << AS_DOUBLE(value) << " ";
  for (auto value : ssb_values_) cache << AS_DOUBLE(value) << " ";
  cache << AS_DOUBLE(r0_) << " " << AS_DOUBLE(b0_) << " " << AS_DOUBLE(steepness_) << " ";
  cache << REPORT_EOL;
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
