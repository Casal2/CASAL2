/**
 * @file RecruitmentBevertonHolt.cpp
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
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
namespace length {

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
  parameters_.Bind<unsigned>(PARAM_SSB_OFFSET, &ssb_offset_, "The spawning biomass year offset", "");
  parameters_.Bind<Double>(PARAM_STEEPNESS, &steepness_, "Steepness (h)", "", 1.0)->set_range(0.0, 1.0);
  parameters_.Bind<string>(PARAM_SSB, &ssb_label_, "The SSB label (i.e., the derived quantity label)", "");
  parameters_.Bind<string>(PARAM_B0_PHASE, &phase_b0_label_, "The initialisation phase label that B0 is from", "", "");
  
  parameters_.Bind<Double>(PARAM_RECRUITMENT_MULTIPLIERS, &recruitment_multipliers_, "The YCS values", "");
  parameters_.Bind<unsigned>(PARAM_STANDARDISE_YEARS, &standardise_years_, "The years that are included for year class standardisation", "", true);
  parameters_.Bind<Double>(PARAM_INITIAL_MEAN_LENGTH, &initial_mean_length_, "Mean length at recruitment for each categories", "");
  parameters_.Bind<Double>(PARAM_INITIAL_LENGTH_CV, &initial_length_cv_, "CV for recruitment of each categories", "");


  // these inputs are deprecated
  parameters_.Bind<unsigned>(PARAM_STANDARDISE_YCS_YEARS, &standardise_ycs_years_, "The years that are included for year class standardisation", "", true);
  parameters_.Bind<Double>(PARAM_YCS_VALUES, &ycs_values_, "The YCS values", "", true);
  parameters_.Bind<unsigned>(PARAM_YCS_YEARS, &ycs_years_, "The recruitment years. A vector of years that relates to the year of the spawning event that created this cohort", "",  true);



  RegisterAsAddressable(PARAM_R0, &r0_);
  RegisterAsAddressable(PARAM_B0, &b0_);
  RegisterAsAddressable(PARAM_STEEPNESS, &steepness_);
  RegisterAsAddressable(PARAM_PROPORTIONS, &proportions_by_category_);
  RegisterAsAddressable(PARAM_RECRUITMENT_MULTIPLIERS, &recruitment_multipliers_by_year_);

  // Allow these to be used in additional priors.
  RegisterAsAddressable(PARAM_STANDARDISED_RECRUITMENT_MULTIPLIERS, &standardised_recruitment_multipliers_by_year_, addressable::kLookup);

  phase_b0_            = 0;
  process_type_        = ProcessType::kRecruitment;
  partition_structure_ = PartitionType::kLength;
}

/**
 * Validate the process
 */
void RecruitmentBevertonHolt::DoValidate() {
  LOG_TRACE();
  // Flag error for deprecated values
  if (parameters_.Get(PARAM_YCS_VALUES)->has_been_defined()) 
    LOG_ERROR_P(PARAM_YCS_VALUES) << PARAM_YCS_VALUES << " is deprecated. The new input is " << PARAM_RECRUITMENT_MULTIPLIERS << ", refer to the user manual for more information";
  if (parameters_.Get(PARAM_YCS_YEARS)->has_been_defined()) 
    LOG_ERROR_P(PARAM_YCS_YEARS) << PARAM_YCS_YEARS << " is deprecated, refer to user manual for more information";
  if (parameters_.Get(PARAM_STANDARDISE_YCS_YEARS)->has_been_defined()) 
    LOG_ERROR_P(PARAM_STANDARDISE_YCS_YEARS) << PARAM_STANDARDISE_YCS_YEARS << " is deprecated. Please use " << PARAM_STANDARDISE_YEARS << " to standardise. Note the years now refer to model years rather than the previous year_class_years. Refer to the user manual for more information";


  if (parameters_.Get(PARAM_R0)->has_been_defined() && parameters_.Get(PARAM_B0)->has_been_defined())
    LOG_FATAL_P(PARAM_R0) << "Cannot specify both R0 and B0 in the model";

  if (!parameters_.Get(PARAM_R0)->has_been_defined() && !parameters_.Get(PARAM_B0)->has_been_defined())
    LOG_FATAL() << "Specify either R0 or B0 to initialise the model with Beverton-Holt recruitment";

  if (category_labels_.size() != proportions_.size())
    LOG_ERROR_P(PARAM_CATEGORIES) << "One proportion is required to be defined per category. There are " << category_labels_.size() << " categories and " << proportions_.size()
                                  << " proportions defined.";
  // if users only give one value assign it to all categories
  if(initial_mean_length_.size() == 1) {
    Double temp = initial_mean_length_[0];
    initial_mean_length_.assign(category_labels_.size(), temp);
  }
  // if users only give one value assign it to all categories
  if(initial_length_cv_.size() == 1) {
    Double temp = initial_length_cv_[0];
    initial_length_cv_.assign(category_labels_.size(), temp);
  }

  if(initial_length_cv_.size() != category_labels_.size())
    LOG_FATAL_P(PARAM_INITIAL_LENGTH_CV) << "There needs to be a value for each category";
  if(initial_mean_length_.size() != category_labels_.size())
    LOG_FATAL_P(PARAM_INITIAL_MEAN_LENGTH) << "There needs to be a value for each category";
  Double running_total = 0.0;
  for (Double value : proportions_)  // Again, ADOLC prevents std::accum
    running_total += value;
  if(!utilities::math::IsOne(running_total))
    LOG_ERROR_P(PARAM_PROPORTIONS) << "The total is " << running_total << " which should be 1.0";

  if (recruitment_multipliers_.size() != model_->years().size()) {
    LOG_FATAL_P(PARAM_RECRUITMENT_MULTIPLIERS) << "There are " << model_->years().size() << " model years and " << recruitment_multipliers_.size() << " " << PARAM_RECRUITMENT_MULTIPLIERS
                                  << " defined. These inputs must be of equal length.";
  }
  // initialise ycs_values and check values arn't < 0.0
  unsigned ycs_iter = 0;
  for (unsigned ycs_year : model_->years()) {
    recruitment_multipliers_by_year_[ycs_year]       = recruitment_multipliers_[ycs_iter];
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
      if(standardise_years_[i] < model_->start_year()) 
        LOG_ERROR_P(PARAM_STANDARDISE_YEARS) << "cannot be less that model start year.";
      if(standardise_years_[i] > model_->final_year()) 
        LOG_ERROR_P(PARAM_STANDARDISE_YEARS) << "cannot be greater than model final year.";
      
      if (standardise_years_[i - 1] >= standardise_years_[i])
        LOG_ERROR_P(PARAM_STANDARDISE_YEARS) << " values must be in strictly increasing order. Value " << standardise_years_[i - 1] << " is not less than "
                                                 << standardise_years_[i];
    }
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
  spawn_event_years_.resize(model_->years().size(),0.0);
  for (unsigned year_iter = 0; year_iter < model_->years().size(); ++year_iter) {
    spawn_event_years_[year_iter] = model_->years()[year_iter] - ssb_offset_;
  }


  // Check users haven't specified a @estimate block for both R0 and B0
  if(IsAddressableUsedFor(PARAM_R0, addressable::kEstimate) & IsAddressableUsedFor(PARAM_B0, addressable::kEstimate))
    LOG_ERROR() << "Both R0 and B0 have an @estimate specified for recruitment process " << label_ << ". Only one of these parameters can be estimated.";
  // Pre allocate memory
  ssb_values_.resize(model_->years().size());
  true_ycs_values_.resize(model_->years().size());
  recruitment_values_.resize(model_->years().size());

  // calculate initial length distribution
  initial_length_distribution_.resize(category_labels_.size());
  DoReset();
}

/**
 * Verify all of the values so they are ready for an execution run
 */
void RecruitmentBevertonHolt::DoVerify(shared_ptr<Model> model) {
  // Check if PARAM_RECRUITMENT_MULTIPLIERS is used
  LOG_FINE() << "check transform usage = " << IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kTransformation);
  LOG_FINE() << "check lookup usage = " << IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kLookup);

  //if (!IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kTransformation)) {
  //   if (!IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kLookup))
  //    LOG_WARNING() << "The parameter " << PARAM_RECRUITMENT_MULTIPLIERS << " has no @additional_prior for it. It is recommended to have a vector_average additional prior";
  if (IsAddressableUsedFor(PARAM_RECRUITMENT_MULTIPLIERS, addressable::kTransformation)) {
    // Check if PARAM_RECRUITMENT_MULTIPLIERS has a transformation if it does don't let users 
    // standardise.
    if (parameters_.Get(PARAM_STANDARDISE_YEARS)->has_been_defined())
      LOG_VERIFY() << "There is an @parameter_transformation  for the parameter " << PARAM_RECRUITMENT_MULTIPLIERS << ", if this is type simplex, you should not specify the subcommand "
                   << PARAM_STANDARDISE_YEARS;
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
  for(unsigned i = 0; i < category_labels_.size(); i++) {
    initial_length_distribution_[i] = utilities::math::distribution(model_->length_bins(), model_->length_plus(), Distribution::kNormal, initial_mean_length_[i], initial_mean_length_[i] * initial_length_cv_[i]);
    Double sum_temp = utilities::math::Sum(initial_length_distribution_[i]);
    // first bin is a min plus group
    initial_length_distribution_[i][0] += 1.0 - sum_temp;
  }
  // if a -i call is made then we need to repopulate the ycs_values vector for reporting.
  // This has to be done because the input parameter ycs_values and registered estimate parameter ycs_values_by_year
  // Are different
  for (unsigned i = 0; i < model_->years().size(); ++i) {
    recruitment_multipliers_[i]                          = recruitment_multipliers_by_year_[model_->years()[i]];
    standardised_recruitment_multipliers_by_year_[model_->years()[i]] = recruitment_multipliers_by_year_[model_->years()[i]];
  }
  unsigned iter = 0;
  for (auto& category : category_labels_) {
    proportions_[iter] = proportions_by_category_[category];
    ++iter;
  }
  // Clear reporting containers
  fill(ssb_values_.begin(), ssb_values_.end(), 0.0);
  fill(true_ycs_values_.begin(), true_ycs_values_.end(), 0.0);
  fill(recruitment_values_.begin(), recruitment_values_.end(), 0.0);

  // Do Haist ycs Parameterisation
  if (standardise_recruitment_multipliers_) {
    Double mean_ycs = 0;
    for (unsigned i = 0; i < model_->years().size(); ++i) {
      for (unsigned j = 0; j < standardise_years_.size(); ++j) {
        if (model_->years()[i] == standardise_years_[j]) {
          mean_ycs += recruitment_multipliers_by_year_[model_->years()[i]];
          break;
        }
      }
    }

    mean_ycs /= standardise_years_.size();
    for (unsigned ycs_year : model_->years()) {
      for (unsigned j = 0; j < standardise_years_.size(); ++j) {
        if (ycs_year == standardise_years_[j])
          standardised_recruitment_multipliers_by_year_[ycs_year] = recruitment_multipliers_by_year_[ycs_year] / mean_ycs;
      }
    }
  } else {
    for (unsigned ycs_year : model_->years()) {
      standardised_recruitment_multipliers_by_year_[ycs_year] = recruitment_multipliers_by_year_[ycs_year];
    }
  }
}

/**
 * Execute this process
 */
void RecruitmentBevertonHolt::DoExecute() {
  unsigned current_year = model_->current_year();
  unsigned ssb_year     = current_year - ssb_offset_;
  std::pair<bool, int> this_year_iter  = niwa::utilities::findInVector(model_->years(), current_year);
  year_counter_ = this_year_iter.second;

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
      Double true_ycs  = 1.0 * ssb_ratio / (1.0 - ((5.0 * steepness_ - 1.0) / (4.0 * steepness_)) * (1.0 - ssb_ratio));
      amount_per       = r0_ * true_ycs;

      LOG_FINEST() << "B0_: " << b0_ << "; ssb_ratio: " << ssb_ratio << "; true_ycs: " << true_ycs << "; amount_per: " << amount_per << " R0 = " << r0_;
    }
    LOG_FINE() << "Initialise: amount_per = " << amount_per;
  } else {
    /**
     * The model is not in an initialisation phase
     */
    LOG_FINEST() <<  "model_->current_year(): " << current_year << "; model_->start_year(): " << model_->start_year();
    Double ycs;
    // If projection mode ycs values get replaced with projected value from @project block
    // note that the container recruitment_multipliers_by_year_ is changed by time_varying and projection classes
    // but the code wants to use standardised_recruitment_multipliers_by_year_ in the functions following here, so we might need to update this.
    if (model_->run_mode() == RunMode::kProjection) {
      if (recruitment_multipliers_by_year_[current_year] == 0.0) {
        LOG_FATAL_P(PARAM_RECRUITMENT_MULTIPLIERS) << "Projection mode (-f) is being run but ycs values are = 0 for ycs_year " << model_->current_year() - ssb_offset_
                                      << ", which will cause the recruitment process to supply 0 recruits. Please check the @project block for this parameter";
      }
      // Projection classes will update this container automatically
      ycs = recruitment_multipliers_by_year_[current_year];
      // We need to see if this value has changed from the initial input, if it has we are going to assume that this is because the projection class has changed it.
      // set standardised ycs = ycs for reporting
      LOG_FINE() << "year = " << ssb_year << " value = " << ycs << " current year " << current_year << " counter = " << year_counter_ << " size of vector " << ycs_values_.size();
      if (ssb_year > model_->final_year()) {
        // we are in projection years so force standardised ycs to be the same as recruitment_multipliers_by_year_[ssb_year];
        standardised_recruitment_multipliers_by_year_[current_year] = ycs;
      } else {
        // we are still within start-final_year need to check if we have overwritten any values.
        if (ycs != ycs_values_[year_counter_])
          standardised_recruitment_multipliers_by_year_[current_year] = ycs;
        else
          ycs = standardised_recruitment_multipliers_by_year_[current_year];
      }
      LOG_FINEST() << "Projected ycs = " << ycs << " what is in the original " << ycs_values_[year_counter_];
      // else business as usual
    } else {
      ycs = standardised_recruitment_multipliers_by_year_[current_year];
      LOG_FINEST() << "ycs" << ycs << " for current year " << current_year;
    }

    // Check whether B0 as an input paramter or a derived quantity, this is a result of having an r0 or a b0 in the process
    if (!parameters_.Get(PARAM_B0)->has_been_defined())
      b0_ = derived_quantity_->GetLastValueFromInitialisation(phase_b0_);

    // Calculate year to get SSB that contributes to this years recruits
    Double SSB;
    if (current_year < model_->start_year()) {
      // Model is in normal years but requires an SSB from the initialisation phase
      initialisationphases::Manager& init_phase_manager = *model_->managers()->initialisation_phase();
      LOG_FINE() << "Initialisation phase index SSB is being extracted from init phase " << init_phase_manager.last_executed_phase() << " current year = " << current_year;
      SSB = derived_quantity_->GetLastValueFromInitialisation(init_phase_manager.last_executed_phase());
    } else {
      // else get value from offset
      SSB = derived_quantity_->GetValue(ssb_year);
    }

    Double ssb_ratio = SSB / b0_;
    Double SR        = ssb_ratio / (1.0 - ((5.0 * steepness_ - 1.0) / (4.0 * steepness_)) * (1.0 - ssb_ratio));
    Double true_ycs  = ycs * SR;
    amount_per       = r0_ * true_ycs;

    true_ycs_values_[year_counter_] = true_ycs;
    recruitment_values_[year_counter_] = amount_per;
    ssb_values_[year_counter_] = SSB;

    LOG_FINEST() << "year = " << model_->current_year() << " SSB = " << SSB << " SR = " << SR << "; ycs = " << recruitment_multipliers_by_year_[current_year]
                 << " Standardised year class = " << standardised_recruitment_multipliers_by_year_[current_year] << "; B0_ = " << b0_ << "; R0 = " << r0_ << "; ssb_ratio = " << ssb_ratio
                 << "; true_ycs = " << true_ycs << "; amount_per = " << amount_per;
  }

  unsigned category_counter = 0;
  for (auto category : partition_) {
    LOG_FINEST() << category->name_ <<  " recruits = " << amount_per << ", proportion of recruits "
                 << proportions_by_category_[category->name_];
    for(unsigned i = 0; i < category->data_.size(); i++)            
      category->data_[i] += amount_per * initial_length_distribution_[category_counter][i] * proportions_by_category_[category->name_];
    ++category_counter;
  }
}

/**
 *  Called in the intialisation phase, this method scales the partition affected by this recruitment event if recruitment is B0 initialised
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
  
  for (auto & category : partition_) {
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
  for (auto iter : spawn_event_years_) 
    cache << iter << " ";
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
  cache << "\nssb_offset: " << ssb_offset_;
  cache << REPORT_EOL;
  for(unsigned i = 0; i < category_labels_.size(); i++) {
    cache << category_labels_[i] << "_initial_length_distribution:";
     for(unsigned j = 0; j < initial_length_distribution_[i].size(); j++)  
      cache << " " << AS_DOUBLE(initial_length_distribution_[i][j]);
    cache << REPORT_EOL;
  }
}

/**
 * Fill the tabular report cache
 */
void RecruitmentBevertonHolt::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {
    vector<unsigned> years = model_->years();
    for (auto iter : standardised_recruitment_multipliers_by_year_) 
      cache << "standardised_recruitment_multipliers[" << iter.first << "] ";
    for (auto iter : recruitment_multipliers_by_year_) 
      cache << "recruitment_multipliers[" << iter.first << "] ";
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

  for (auto iter : standardised_recruitment_multipliers_by_year_) cache << AS_DOUBLE(iter.second) << " ";
  for (auto iter : recruitment_multipliers_by_year_) cache << AS_DOUBLE(iter.second) << " ";

  for (auto value : true_ycs_values_) cache << AS_DOUBLE(value) << " ";
  for (auto value : recruitment_values_) cache << AS_DOUBLE(value) << " ";
  for (auto value : ssb_values_) cache << AS_DOUBLE(value) << " ";

  cache << AS_DOUBLE(r0_) << " " << AS_DOUBLE(b0_) << " " << AS_DOUBLE(steepness_) << " ";
  cache << REPORT_EOL;
}

} /* namespace length */
} /* namespace processes */
} /* namespace niwa */
