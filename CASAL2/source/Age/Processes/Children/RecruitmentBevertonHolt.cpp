/**
 * @file RecruitmentBevertonHolt.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 12/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "RecruitmentBevertonHolt.h"

#include <numeric>
#include <limits>

#include "Common/Categories/Categories.h"
#include "Common/DerivedQuantities/Manager.h"
#include "Common/InitialisationPhases/Manager.h"
#include "Common/Estimates/Manager.h"
#include "Common/TimeSteps/Manager.h"
#include "Common/Utilities/DoubleCompare.h"
#include "Common/Utilities/Math.h"
#include "Common/Utilities/To.h"

// namespaces
namespace niwa {
namespace age {
namespace processes {

namespace dc = niwa::utilities::doublecompare;
namespace math = niwa::utilities::math;

/**
 * default constructor
 */
RecruitmentBevertonHolt::RecruitmentBevertonHolt(Model* model)
  : Process(model),
    partition_(model) {
  LOG_TRACE();

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "Category labels", "");
  parameters_.Bind<Double>(PARAM_R0, &r0_, "R0", "",false);
  parameters_.Bind<Double>(PARAM_B0, &b0_, "B0", "",false);
  parameters_.Bind<Double>(PARAM_PROPORTIONS, &proportions_, "Proportions", "");
  parameters_.Bind<unsigned>(PARAM_AGE, &age_, "Age to recruit at", "", true);
  parameters_.Bind<unsigned>(PARAM_SSB_OFFSET, &ssb_offset_, "Spawning biomass year offset","", true);
  parameters_.Bind<Double>(PARAM_STEEPNESS, &steepness_, "Steepness", "", 1.0);
  parameters_.Bind<string>(PARAM_SSB, &ssb_, "SSB Label (derived quantity)", "");
  parameters_.Bind<string>(PARAM_B0_PHASE, &phase_b0_label_, "Initialisation phase Label that b0 is from", "", "");
  parameters_.Bind<Double>(PARAM_YCS_VALUES, &ycs_values_, "YCS Values", "");
  parameters_.Bind<unsigned>(PARAM_YCS_YEARS, &ycs_years_, "Recruitment years. A vector of years that relates to the year of the spawning event that created this cohort", "", false);
  parameters_.Bind<unsigned>(PARAM_STANDARDISE_YCS_YEARS, &standardise_ycs_, "Years that are included for year class standardisation", "", true);

  RegisterAsAddressable(PARAM_R0, &r0_);
  RegisterAsAddressable(PARAM_B0, &b0_);
  RegisterAsAddressable(PARAM_STEEPNESS, &steepness_);
  RegisterAsAddressable(PARAM_PROPORTIONS, &proportions_);
  RegisterAsAddressable(PARAM_YCS_VALUES, &ycs_value_by_year_);

  // Allow these to be used in additional priors.
  RegisterAsAddressable(PARAM_STANDARDISE_YCS_YEARS, &stand_ycs_value_by_year_,addressable::kLookup);


  phase_b0_         = 0;
  process_type_     = ProcessType::kRecruitment;
  partition_structure_ = PartitionType::kAge;
}

/**
 *
 */
void RecruitmentBevertonHolt::DoValidate() {
  LOG_TRACE();
  category_labels_ = model_->categories()->ExpandLabels(category_labels_, parameters_.Get(PARAM_CATEGORIES));
  if (!parameters_.Get(PARAM_AGE)->has_been_defined())
    age_ = model_->min_age();

  if (parameters_.Get(PARAM_R0)->has_been_defined() & parameters_.Get(PARAM_B0)->has_been_defined())
    LOG_FATAL_P(PARAM_R0) << "Cannot specify both R0 and B0 in the model";

  if (!parameters_.Get(PARAM_R0)->has_been_defined() & !parameters_.Get(PARAM_B0)->has_been_defined())
    LOG_FATAL() << "You need to specify either R0 or B0 to intialise the model with Beverton Holt recruitment";

  // Ensure defined categories were valid
  for(const string& category : category_labels_) {
    if (!model_->categories()->IsValid(category))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": category " << category << " is not a valid category";
  }

  if (age_ < model_->min_age())
    LOG_ERROR_P(PARAM_AGE) << " (" << age_ << ") cannot be less than the model's min_age (" << model_->min_age() << ")";
  if (age_ > model_->max_age())
    LOG_ERROR_P(PARAM_AGE) << " (" << age_ << ") cannot be greater than the model's max_age (" << model_->max_age() << ")";

  if (category_labels_.size() != proportions_.size())
    LOG_ERROR_P(PARAM_CATEGORIES) << " defined need 1 proportion defined per category. There are " << category_labels_.size()
        << " categories and " << proportions_.size() << " proportions defined.";

  Double running_total = 0.0;
  for (Double value : proportions_) // Again, ADOLC prevents std::accum
    running_total += value;
  if (!dc::IsOne(running_total))
    LOG_ERROR_P(PARAM_PROPORTIONS) << " sum total is " << running_total << " when it should be 1.0";

  if (ycs_years_.size() != ((model_->final_year() - model_->start_year()) + 1))
    LOG_ERROR_P(PARAM_YCS_YEARS) << "There must be a year class year for each year of the model";

  if(ycs_values_.size() != ycs_years_.size()) {
    LOG_FATAL_P(PARAM_YCS_VALUES) << "you supplied " << ycs_years_.size() << " " << PARAM_YCS_YEARS  << " and " << ycs_values_.size() << " " << PARAM_YCS_VALUES << ". These parameters must be of equal length before the model will run.";
  }
  // initialise ycs_values and check values arn't < 0.0
  unsigned ycs_iter = 0;
  for (unsigned ycs_year : ycs_years_) {
    ycs_value_by_year_[ycs_year] = ycs_values_[ycs_iter];
    stand_ycs_value_by_year_[ycs_year] = ycs_values_[ycs_iter];
    if (ycs_values_[ycs_iter] < 0.0)
        LOG_ERROR_P(PARAM_YCS_VALUES) << " value " << ycs_values_[ycs_iter] << " cannot be less than 0.0";
    ycs_iter++;
  }


  // Check ascending order
  if (standardise_ycs_.size() == 0) {
    standardise_ycs_ = ycs_years_;
  } else if (standardise_ycs_.size() > 1) {
    for (unsigned i = 1; i < standardise_ycs_.size(); ++i) {
      if (standardise_ycs_[i - 1] >= standardise_ycs_[i])
        LOG_ERROR_P(PARAM_STANDARDISE_YCS_YEARS) << " values must be in numeric ascending order. Value "
            << standardise_ycs_[i - 1] << " is not less than " << standardise_ycs_[i];
    }
  }

  for (unsigned i = 1; i < ycs_years_.size(); ++i) {
    if (ycs_years_[i - 1] >= ycs_years_[i])
      LOG_ERROR_P(PARAM_YCS_VALUES) << " values must be in numeric ascending order. Value "
          << ycs_years_[i - 1] << " is not less than " << ycs_years_[i];
  }
}

/**
 * Build the runtime relationships between this object and it's
 */
void RecruitmentBevertonHolt::DoBuild() {
  partition_.Init(category_labels_);

  if (parameters_.Get(PARAM_B0)->has_been_defined()) {
    model_->set_b0_initialised(ssb_, true);
    bo_initialised_ = true;
  }
  if (phase_b0_label_ != "")
    phase_b0_ = model_->managers().initialisation_phase()->GetPhaseIndex(phase_b0_label_);

  derived_quantity_ = model_->managers().derived_quantity()->GetDerivedQuantity(ssb_);
  if (!derived_quantity_)
    LOG_ERROR_P(PARAM_SSB) << "(" << ssb_ << ") could not be found. Have you defined the right @derived_quantity?";

  /**
   * Calculate out SSB offset
   */
  unsigned temp_ssb_offset = 0;
  const vector<TimeStep*> ordered_time_steps = model_->managers().time_step()->ordered_time_steps();
  unsigned time_step_index = 0;
  unsigned process_index = 0;
  unsigned ageing_processes = 0;
  unsigned ageing_index = std::numeric_limits<unsigned>::max();
  unsigned recruitment_index = std::numeric_limits<unsigned>::max();
  unsigned derived_quantity_index = std::numeric_limits<unsigned>::max();
  unsigned derived_quantity_time_step_index = model_->managers().time_step()->GetTimeStepIndex(derived_quantity_->time_step());
  bool mortailty_block = false;

  // loop through time steps
  for (auto time_step : ordered_time_steps) {
    if (time_step_index == derived_quantity_time_step_index) {
      for (auto process : time_step->processes()) {
        if (process->process_type() == ProcessType::kAgeing) {
          ageing_index = process_index;
          ageing_processes++;
        }
        if (process->process_type() == ProcessType::kMortality) {

          mortailty_block = true;
          derived_quantity_index = process_index;
        }
        process_index++;
      }
      LOG_FINEST() << "process_index = " << process_index;
      if (!mortailty_block) {
        process_index++;
        LOG_FINEST() << "Are we entering this loop?" << process_index;
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
  recruitment_index = model_->managers().time_step()->GetProcessIndex(label_);
  if (ageing_processes > 1)
    LOG_ERROR_P(PARAM_SSB_OFFSET) << "BH recruitment year offset has been calculated on the basis of a single ageing process. We have identified "
        << ageing_processes << " ageing processes, we suggest manually setting ssb_offset for this scenerio. Or contacting the development team to discuss a change";
  if (ageing_index == std::numeric_limits<unsigned>::max())
    LOG_ERROR() << location() << " could not calculate the ssb_offset because there is no ageing process";

  if (recruitment_index < ageing_index && ageing_index < derived_quantity_index)
    temp_ssb_offset = age_ + 1;
  else if (derived_quantity_index < ageing_index && ageing_index < recruitment_index)
    temp_ssb_offset = age_ - 1;
  else
    temp_ssb_offset = age_;

  LOG_FINEST() << "SSB Offset calculated to be = " << temp_ssb_offset << "; recruitment index = " << recruitment_index << "; ageing index = "
      << ageing_index << "; derived_quantity index = " << derived_quantity_index;
  if (parameters_.Get(PARAM_SSB_OFFSET)->has_been_defined()) {
    // Check if the user has supplied the expected value for the model.
    if (temp_ssb_offset != ssb_offset_) {
      LOG_WARNING() << "You have specified a different " << PARAM_SSB_OFFSET << " to what Casal2 calculated. Make sure you understand how this parameter is used in the process before you manually set it. Casal2 will automatically generate this variable, and should only be manually set under certain scenerios. See the usermanual on this process for more information.";
    }
  }else {
    ssb_offset_ = temp_ssb_offset;
  }

  // Check that ycs years corresponds to the correct ssb_offset calculated
  if (ycs_years_[0] != (model_->start_year() - ssb_offset_))
    LOG_ERROR_P(PARAM_YCS_YEARS) << "The first year class year you supplied is " << ycs_years_[0] << ", but with your configuration of the process it should be " << model_->start_year() - ssb_offset_ <<  " please check either the ycs_year parameter or see the manual for more information on how Casal2 calculates this value.";
  if (ycs_years_[ycs_years_.size() - 1] != (model_->final_year() - ssb_offset_))
    LOG_ERROR_P(PARAM_YCS_YEARS) << "The last year class year you supplied is " << ycs_years_[ycs_years_.size() - 1] << ", but with your configuration of the process it should be " << model_->final_year() - ssb_offset_ <<  " please check either the ycs_year parameter or see the manual for more information on how Casal2 calculates this value.";

  if (standardise_ycs_[0] < model_->start_year() - ssb_offset_)
    LOG_ERROR_P(PARAM_STANDARDISE_YCS_YEARS) << " first value is less than the model's start_year - ssb offset = " << model_->start_year() - ssb_offset_;
  if ((*standardise_ycs_.rbegin()) > model_->final_year() - ssb_offset_)
    LOG_ERROR_P(PARAM_STANDARDISE_YCS_YEARS) << " final value (" << (*standardise_ycs_.rbegin())
        << ") is greater than the model's final year - ssb_offset (" << model_->final_year() - ssb_offset_ << ")";

  // Check users haven't specified a @estimate block for both R0 and B0
  string b0_param = "process[" + label_ + "].b0";
  string r0_param = "process[" + label_ + "].r0";

  bool B0_estimate = model_->managers().estimate()->HasEstimate(b0_param);
  bool R0_estimate = model_->managers().estimate()->HasEstimate(r0_param);

  LOG_FINEST() << "is b0 estimated = " << B0_estimate << " is R0 estimated " << R0_estimate;
  if(B0_estimate & R0_estimate) {
    LOG_ERROR() << "Found an @estimate for both R0 and B0 for recruitment process " << label_ << " this is not allowed, you can only estimate one of these parameters";
  }

  // Pre allocate memory
  ssb_values_.reserve(model_->years().size());
  true_ycs_values_.reserve(model_->years().size());
  recruitment_values_.reserve(model_->years().size());

  DoReset();
}

/**
 * Reset all of the values so they're ready for an execution run
 */
void RecruitmentBevertonHolt::DoReset() {
  LOG_TRACE();

  if (parameters_.Get(PARAM_B0)->has_been_defined()) {
    have_scaled_partition = false;
  }

  // if a -i call is made then we need to repopulate the ycs_values vector for reporting. This has to be done because the input parameter ycs_values and registered estimate parameter ycs_values_by_year
  // Are different
  for (unsigned i = 0; i < ycs_years_.size(); ++i) {
    ycs_values_[i] = ycs_value_by_year_[ycs_years_[i]];
    stand_ycs_value_by_year_[ycs_years_[i]] = ycs_value_by_year_[ycs_years_[i]];
  }

  ssb_values_.clear();
  true_ycs_values_.clear();
  recruitment_values_.clear();

  // Do Haist ycs Parameterisation
  Double mean_ycs = 0;
  for (unsigned i = 0; i < ycs_years_.size(); ++i) {
    for (unsigned j = 0; j < standardise_ycs_.size(); ++j) {
      if (ycs_years_[i] == standardise_ycs_[j]) {
        mean_ycs += ycs_value_by_year_[ycs_years_[i]];
        break;
      }
    }
  }

  mean_ycs /= standardise_ycs_.size();
  for (unsigned ycs_year : ycs_years_) {
      for (unsigned j = 0; j < standardise_ycs_.size(); ++j) {
      if (ycs_year == standardise_ycs_[j])
        stand_ycs_value_by_year_[ycs_year] = ycs_value_by_year_[ycs_year] / mean_ycs;
    }
  }
  year_counter_ = 0;
}

/**
 * Execute this process.
 */

void RecruitmentBevertonHolt::DoExecute() {
  unsigned ssb_year = model_->current_year() - ssb_offset_;

  Double amount_per = 0.0;
  if (model_->state() == State::kInitialise) {
    initialisationphases::Manager& init_phase_manager = *model_->managers().initialisation_phase();
    if ((init_phase_manager.last_executed_phase() <= phase_b0_) & (parameters_.Get(PARAM_R0)->has_been_defined())) {
      amount_per = r0_;
    } else if ((init_phase_manager.last_executed_phase() <= phase_b0_) & (parameters_.Get(PARAM_B0)->has_been_defined())) {
      if (have_scaled_partition)
        amount_per = r0_;
      else
        amount_per = 1;
    } else {
      // if R0 intialised mode then b0 is a derived quantity
      if (!parameters_.Get(PARAM_B0)->has_been_defined())
        b0_ = derived_quantity_->GetLastValueFromInitialisation(phase_b0_);

      Double SSB = derived_quantity_->GetLastValueFromInitialisation(init_phase_manager.last_executed_phase());
      Double ssb_ratio = SSB / b0_;
      Double true_ycs = 1.0 * ssb_ratio / (1 - ((5 * steepness_ - 1) / (4 * steepness_)) * (1 - ssb_ratio));
      amount_per = r0_ * true_ycs;

      LOG_FINEST() << "b0_: " << b0_ << "; ssb_ratio: " << ssb_ratio << "; true_ycs: " << true_ycs << "; amount_per: " << amount_per << " R0 = " << r0_;
    }

  } else {
    /**
     * The model is not in an initialisation phase
     */
    ++year_counter_;
    LOG_FINEST() << "standardise_ycs_.size(): " << standardise_ycs_.size() << "; model_->current_year(): " << model_->current_year()
        << "; model_->start_year(): " << model_->start_year();
    Double ycs;
    // If projection mode ycs values get replaced with projected value from @project block
    if (model_->run_mode() == RunMode::kProjection) {
      // An issue with this is we can start projecting values before final_year(), how can we catch that.
      if (ycs_value_by_year_[ssb_year] == 0.0) {
        LOG_FATAL_P(PARAM_YCS_VALUES) << "You are in a projection mode (-f) and in a projection year but ycs values are = 0 for ycs_year " << model_->current_year() - ssb_offset_ << ", this is an error that will cause the recruitment process to fail. Please check you have correctly specified an @project block for this parameter, thanks";
      }
      ycs = ycs_value_by_year_[ssb_year];
      // We need to see if this value has changed from the initial input, if it has we are going to assume that this is because the projection class has changed it.
      // set standardised ycs = ycs for reporting

      if (ycs != ycs_values_[year_counter_])
        stand_ycs_value_by_year_[ssb_year] = ycs;
      else
        ycs = stand_ycs_value_by_year_[ssb_year];

      LOG_FINEST() << "Projected ycs = " << ycs;
    // else business as usual
    } else {
      ycs = stand_ycs_value_by_year_[ssb_year];
    }

    // Check whether B0 as an input paramter or a derived quantity, this is a result of having an r0 or a b0 in the process
    if (!parameters_.Get(PARAM_B0)->has_been_defined())
      b0_ = derived_quantity_->GetLastValueFromInitialisation(phase_b0_);

    // Calculate year to get SSB that contributes to this years recruits
    Double SSB;
    if (ssb_year < model_->start_year()) {
      // Model is in normal years but requires an SSB from the initialisation phase
      initialisationphases::Manager& init_phase_manager = *model_->managers().initialisation_phase();
      LOG_FINE() << "Initialisation phase index SSB is being extracted from init phase " << init_phase_manager.last_executed_phase()
          << " SSB year = " << ssb_year;
      SSB = derived_quantity_->GetLastValueFromInitialisation(init_phase_manager.last_executed_phase());
    } else {
      // else get value from offset
      SSB = derived_quantity_->GetValue(ssb_year);
    }
    Double ssb_ratio = SSB / b0_;
    Double SR = ssb_ratio / (1.0 - ((5.0 * steepness_ - 1.0) / (4.0 * steepness_)) * (1.0 - ssb_ratio));
    Double true_ycs = ycs * SR;
    amount_per = r0_ * true_ycs;

    true_ycs_values_.push_back(true_ycs);
    recruitment_values_.push_back(amount_per);
    ssb_values_.push_back(SSB);


    LOG_FINEST() << "year = " << model_->current_year() << " SSB= " << SSB << " SR = " << SR << "; ycs = "
        << ycs_value_by_year_[ssb_year] << " Standardised year class = "
        << stand_ycs_value_by_year_[ssb_year] << "; b0_ = " << b0_ << "; ssb_ratio = " << ssb_ratio << "; true_ycs = "
        << true_ycs << "; amount_per = " << amount_per;
  }

  unsigned i = 0;
  for (auto category : partition_) {
    LOG_FINEST() << category->name_ << "; age: " << age_ << "; category->min_age_: " << category->min_age_ << " recruits = " << amount_per;
    category->data_[age_ - category->min_age_] += amount_per * proportions_[i];
    ++i;
  }
}

/**
 *  Called in the intialisation phase, this method while scale the partition effected by this recruitment event if recruitment is B0 initialised
 */
void RecruitmentBevertonHolt::ScalePartition() {
  if (!parameters_.Get(PARAM_B0)->has_been_defined())
    LOG_CODE_ERROR() << "Cannot apply this method as B0 has not been defined";

  have_scaled_partition = true;
  Double alternative_ssb = derived_quantity_->GetValue(model_->start_year() - ssb_offset_);
  // Look at initialisation phase
  Double SSB = derived_quantity_->GetLastValueFromInitialisation(phase_b0_);
  LOG_FINEST() << "Last SSB value = " << SSB << " init ssb = " << alternative_ssb;
  Double scalar = b0_ / SSB;
  LOG_FINEST() << "Scalar = " << scalar << " B0 = " << b0_;
  LOG_FINEST() << "r0 = " << scalar;
  r0_ = scalar;
  for (auto category : partition_) {
    for (unsigned j = 0; j < category->data_.size(); ++j) {
      LOG_FINEST() << "Category "<< category->name_ << " Age = " << j + category->min_age_ << " Numbers at age before = " <<  category->data_[j];
      category->data_[j] *= scalar;
      LOG_FINEST() << "Category "<< category->name_ << " Age = " << j + category->min_age_ << " Numbers at age = " <<  category->data_[j];
    }
  }
  LOG_FINEST() << "R0 = " << r0_;
}

/**
 *
 */
void RecruitmentBevertonHolt::FillReportCache(ostringstream& cache) {
  cache << "ycs_years: ";
  for (auto iter : stand_ycs_value_by_year_)
    cache << iter.first << " ";
  cache << "\nstandardiesed_ycs: ";
  for (auto iter : stand_ycs_value_by_year_)
    cache << iter.second << " ";
  cache << "\nycs_values: ";
  for (auto iter : ycs_value_by_year_)
    cache << iter.second << " ";
  cache << "\ntrue_ycs: ";
  for (auto iter : true_ycs_values_)
    cache << iter << " ";
  cache << "\nRecruits: ";
  for (auto iter : recruitment_values_)
    cache << iter << " ";
  cache << "\nSSB: ";
  for (auto iter : ssb_values_)
    cache << iter << " ";
}

/**
 *
 */
void RecruitmentBevertonHolt::FillTabularReportCache(ostringstream& cache, bool first_run) {
  if (first_run) {
    vector<unsigned> years = model_->years();
    for (auto year : years) {
      unsigned ssb_year = year - ssb_offset_;
      cache << "standardiesed_ycs[" << ssb_year << "] ";
    }
    for (auto year : years) {
      unsigned ssb_year = year - ssb_offset_;
      cache <<  "true_ycs[" << ssb_year << "] ";
    }
    for (auto year : years) {
      unsigned ssb_year = year - ssb_offset_;
      cache << "recruits["<< ssb_year << "] ";
    }
    cache << "R0 B0 steepness ";
    cache << "\n";
  }

  for (auto iter : stand_ycs_value_by_year_)
    cache << iter.second << " ";
  for (auto value : true_ycs_values_)
    cache << value << " ";
  for (auto value : recruitment_values_)
    cache << value << " ";
  cache << r0_ << " " << b0_ << " " << steepness_ << " ";
}

} /* namespace processes */
} /* namespace age */
} /* namespace niwa */
