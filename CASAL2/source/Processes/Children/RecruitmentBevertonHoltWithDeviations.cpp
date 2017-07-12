/**
 * @file RecruitmentBevertonHoltWithDeviations.cpp
 * @author C.Marsh
 * @date 11/7/17
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 *
 */

// headers
#include "RecruitmentBevertonHoltWithDeviations.h"

#include <numeric>
#include <limits>

#include "Categories/Categories.h"
#include "DerivedQuantities/Manager.h"
#include "InitialisationPhases/Manager.h"
#include "Estimates/Manager.h"
#include "TimeSteps/Manager.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/Math.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace processes {

namespace dc = niwa::utilities::doublecompare;
namespace math = niwa::utilities::math;

/**
 * default constructor
 */
RecruitmentBevertonHoltWithDeviations::RecruitmentBevertonHoltWithDeviations(Model* model)
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
  parameters_.Bind<Double>(PARAM_SIGMA_R, &sigma_r_, "Sigma r", "");
  parameters_.Bind<string>(PARAM_B0_PHASE, &phase_b0_label_, "Initialisation phase Label that b0 is from", "", "");
  parameters_.Bind<Double>(PARAM_DEVIATION_VALUES, &recruit_dev_values_, "Recruitment deviation values", "");
  parameters_.Bind<unsigned>(PARAM_DEVIATION_YEARS, &recruit_dev_years_, "Recruitment years. A vector of years that relates to the year of the spawning event that created this cohort", "", false);

  RegisterAsAddressable(PARAM_R0, &r0_);
  RegisterAsAddressable(PARAM_B0, &b0_);
  RegisterAsAddressable(PARAM_SIGMA_R, &sigma_r_);
  RegisterAsAddressable(PARAM_STEEPNESS, &steepness_);
  RegisterAsAddressable(PARAM_PROPORTIONS, &proportions_);
  RegisterAsAddressable(PARAM_DEVIATION_VALUES, &recruit_dev_value_by_year_);

  // Allow these to be used in additional priors.


  phase_b0_         = 0;
  process_type_     = ProcessType::kRecruitment;
  partition_structure_ = PartitionStructure::kAge;
}

/**
 *
 */
void RecruitmentBevertonHoltWithDeviations::DoValidate() {
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

  if (recruit_dev_values_.size() != ((model_->final_year() - model_->start_year()) + 1))
    LOG_ERROR_P(PARAM_DEVIATION_YEARS) << "There must be a year class year for each year of the model";

  if(recruit_dev_values_.size() != recruit_dev_years_.size()) {
    LOG_FATAL_P(PARAM_DEVIATION_VALUES) << "you supplied " << recruit_dev_years_.size() << " " << PARAM_DEVIATION_YEARS  << " and " << recruit_dev_values_.size() << " " << PARAM_YCS_VALUES << ". These parameters must be of equal length before the model will run.";
  }
  // initialise ycs_values and check values arn't < 0.0
  unsigned ycs_iter = 0;
  for (unsigned ycs_year : recruit_dev_years_) {
    recruit_dev_value_by_year_[ycs_year] = recruit_dev_values_[ycs_iter];
    if (recruit_dev_values_[ycs_iter] < 0.0)
        LOG_ERROR_P(PARAM_DEVIATION_YEARS) << " value " << recruit_dev_values_[ycs_iter] << " cannot be less than 0.0";
    ycs_iter++;
  }

  // Check years are in incremental ascending order
  for (unsigned i = 1; i < recruit_dev_years_.size(); ++i) {
    if ((recruit_dev_years_[i - 1] + 1) !=  recruit_dev_years_[i])
      LOG_ERROR_P(PARAM_DEVIATION_YEARS) << " values must be in incremental ascending order. Value " << recruit_dev_years_[i - 1] << " + 1 does not equal " << recruit_dev_years_[i];
  }

}

/**
 * Build the runtime relationships between this object and it's
 */
void RecruitmentBevertonHoltWithDeviations::DoBuild() {
  partition_.Init(category_labels_);

  if (parameters_.Get(PARAM_B0)->has_been_defined()) {
    model_->set_b0_initialised(ssb_, true);
    bo_initialised_ = true;
  }
  if (phase_b0_label_ != "")
    phase_b0_ = model_->managers().initialisation_phase()->GetPhaseIndex(phase_b0_label_);

  derived_quantity_ = model_->managers().derived_quantity()->GetDerivedQuantity(ssb_);
  if (!derived_quantity_)
    LOG_ERROR_P(PARAM_SSB) << "(" << ssb_ << ") could not be found. Have you defined it?";

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
  if (recruit_dev_years_[0] != (model_->start_year() - ssb_offset_))
    LOG_ERROR_P(PARAM_DEVIATION_YEARS) << "The first year class year you supplied is " << recruit_dev_years_[0] << ", but with your configuration of the process it should be " << model_->start_year() - ssb_offset_ <<  " please check either the ycs_year parameter or see the manual for more information on how Casal2 calculates this value.";
  if (recruit_dev_years_[recruit_dev_years_.size() - 1] != (model_->final_year() - ssb_offset_))
    LOG_ERROR_P(PARAM_DEVIATION_YEARS) << "The last year class year you supplied is " << recruit_dev_years_[recruit_dev_years_.size() - 1] << ", but with your configuration of the process it should be " << model_->final_year() - ssb_offset_ <<  " please check either the ycs_year parameter or see the manual for more information on how Casal2 calculates this value.";

  // Check users haven't specified a @estimate block for both R0 and B0
  string b0_param = "process[" + label_ + "].b0";
  string r0_param = "process[" + label_ + "].r0";

  bool B0_estimate = model_->managers().estimate()->HasEstimate(b0_param);
  bool R0_estimate = model_->managers().estimate()->HasEstimate(r0_param);

  LOG_FINEST() << "is b0 estimated = " << B0_estimate << " is R0 estimated " << R0_estimate;
  if(B0_estimate & R0_estimate) {
    LOG_ERROR() << "Found an @estimate for both R0 and B0 for recruitment process " << label_ << " this is not allowed, you can only estimate one of these parameters";
  }

  // Check if recruitment devs have an @estimate block, I am just checking over the first year
  for (auto year : recruit_dev_years_) {
    string year_string;
    if (!utilities::To<unsigned, string>(year, year_string))
      LOG_CODE_ERROR() << "Could not convert the value " << year << " to a string";

    string recruit_parm = "process[" + label_ + "]." + PARAM_DEVIATION_VALUES + "{" + year_string + "}";
    if (model_->managers().estimate()->HasEstimate(recruit_parm)) {
      Estimate* recruit_dev_estimate = model_->managers().estimate()->GetEstimate(recruit_parm);
      if (!recruit_dev_estimate)
        LOG_CODE_ERROR() << "'!sigma_r_estimate_', parameter " << recruit_parm << " has estimate but we cannot get the estimate";
      if (recruit_dev_estimate->type() != PARAM_NORMAL_BY_STDEV)
        LOG_ERROR() << "Found an @estimate block for " << recruit_parm << " that is not of type " << PARAM_NORMAL_BY_STDEV << ". Currently you can only have @estimate block for deviation_values that are of type " << PARAM_NORMAL_BY_STDEV;
      // Check sigma is the same as sigma_r
      map<string, Parameter*> parameters = recruit_dev_estimate->parameters().parameters();
      for (auto param = parameters.begin(); param != parameters.end(); ++param) {
        if(param->first == PARAM_SIGMA) {
          Double estimate_sigma = 0.0;
          if (!utilities::To<string, Double>(param->second->values()[0], estimate_sigma))
            LOG_CODE_ERROR() << "Could not convert the value " << param->second->values()[0] << " to a Double";

          if (fabs(estimate_sigma - sigma_r_) > 0.001) {
            LOG_FATAL_P(PARAM_SIGMA_R) << "Found an @estiamte block for parameter " << recruit_parm << " which had a sigma = " << estimate_sigma << ", you specified a sigma = " << sigma_r_ << " in this process. These two sigma's must be the sane, please change one of them";
          }
        }
      }
    }
  }

  DoReset();
}

/**
 * Reset all of the values so they're ready for an execution run
 * - check if we need to rescale partition
 * - update input parameters to updated parameters
 * - clear reporting containers
 * - check where b0 is coming from.
 */
void RecruitmentBevertonHoltWithDeviations::DoReset() {
  LOG_TRACE();
  if (parameters_.Get(PARAM_B0)->has_been_defined()) {
    have_scaled_partition = false;
  }
  // if a -i call is made then we need to re-populate the recruit_dev_values_ vector for reporting. This has to be done because the input parameter recruit_dev_values_ and registered estimate parameter recruit_dev_value_by_year_
  // Are different
  for (unsigned i = 0; i < recruit_dev_years_.size(); ++i) {
    recruit_dev_values_[i] = recruit_dev_value_by_year_[recruit_dev_years_[i]];
  }

  // clear containers for another annual cycle.
  ssb_values_.clear();
  true_ycs_values_.clear();
  recruitment_values_.clear();

  // Check whether B0 as an input paramter or a derived quantity, this is a result of having an r0 or a b0 in the process
  // if its estimated or an input it will be updates.
  if (!parameters_.Get(PARAM_B0)->has_been_defined())
    b0_ = derived_quantity_->GetLastValueFromInitialisation(phase_b0_);
}

/**
 * Execute this process.
 *
 */

void RecruitmentBevertonHoltWithDeviations::DoExecute() {
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

      LOG_FINEST() << "b0_: " << b0_ << "; ssb_ratio: " << ssb_ratio << "; true_ycs: " << true_ycs << "; amount_per: " << amount_per;
    }

  } else {
    /**
     * The model is not in an initialisation phase
     */
    LOG_FINEST() << "; model_->start_year(): " << model_->start_year();
    Double ycs;
    ycs = exp(recruit_dev_value_by_year_[ssb_year] - 0.5 * sigma_r_ * sigma_r_);
    LOG_FINEST() << "Projected ycs = " << ycs;


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

    LOG_FINEST() << "year = " << model_->current_year() << " SSB= " << SSB << " SR = " << SR << "; recruit_dev = "
        << recruit_dev_value_by_year_[ssb_year] << "; b0_ = " << b0_ << "; ssb_ratio = " << ssb_ratio << "; true_ycs = "
        << true_ycs << "; amount_per = " << amount_per;

    // Store true_ycs values
    StoreForReport("deviation_years: " , ssb_year); // the input parameter isn't updated during projections. So thats why we are reporting it twice.
    StoreForReport("recruit_devs: " , AS_DOUBLE(recruit_dev_value_by_year_[ssb_year])); // the input parameter isn't updated during projections. So thats why we are reporting it twice.
    StoreForReport("ycs: " , AS_DOUBLE(ycs));
    StoreForReport("true_ycs: " , AS_DOUBLE(true_ycs)); // this is including SR-relationship

    // Store for Tabular report
    string ssb_year_string;
    if (!utilities::To<unsigned, string>(ssb_year, ssb_year_string))
      LOG_CODE_ERROR() << "Could not convert the value " << ssb_year << " to a string for storage in the tabular report";

    string stand_label = "recruit_dev." + ssb_year_string;
    string true_ycs_label = "true_ycs." + ssb_year_string;
    string ycs_label = "ycs." + ssb_year_string;

    LOG_FINEST() << "adding tabular report = " << true_ycs_label;

    StoreForTabularReport(stand_label, AS_DOUBLE(recruit_dev_value_by_year_[ssb_year]));
    StoreForTabularReport(true_ycs_label ,  AS_DOUBLE(true_ycs));
    StoreForTabularReport(ycs_label ,  AS_DOUBLE(ycs));


  }

  unsigned i = 0;
  for (auto category : partition_) {
    LOG_FINEST() << category->name_ << "; age: " << age_ << "; category->min_age_: " << category->min_age_;
    category->data_[age_ - category->min_age_] += amount_per * proportions_[i];
    ++i;
  }
}

/**
 *  Called in the intialisation phase, this method while scale the partition effected by this recruitment event if recruitment is B0 initialised
 */
void RecruitmentBevertonHoltWithDeviations::ScalePartition() {
  if (!parameters_.Get(PARAM_B0)->has_been_defined())
    LOG_CODE_ERROR() << "Cannot apply this method as B0 has not been defined";

  have_scaled_partition = true;
  Double SSB = derived_quantity_->GetValue(model_->start_year() - ssb_offset_);
  LOG_FINEST() << "Last SSB value = " << SSB;
  Double scalar = b0_ / SSB;
  LOG_FINEST() << "Scalar = " << scalar << " B0 = " << b0_;
  LOG_FINEST() << "r0 = " << scalar;
  r0_ = scalar;
  for (auto category : partition_) {
    for (unsigned j = 0; j < category->data_.size(); ++j) {
      LOG_FINEST() << "Category "<< category->name_ << " Age = " << j + category->min_age_ << " Numbers at age = " <<  category->data_[j];
      category->data_[j] *= scalar;
      LOG_FINEST() << "Category "<< category->name_ << " Age = " << j + category->min_age_ << " Numbers at age = " <<  category->data_[j];
    }
  }
  LOG_FINEST() << "R0 = " << r0_;
}


} /* namespace processes */
} /* namespace niwa */
