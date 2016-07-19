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

#include "Categories/Categories.h"
#include "DerivedQuantities/Manager.h"
#include "InitialisationPhases/Manager.h"
#include "TimeSteps/Manager.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/Math.h"

// namespaces
namespace niwa {
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
  parameters_.Bind<int>(PARAM_SSB_OFFSET, &ssb_offset_, "Spawning biomass year offset","", false);
  parameters_.Bind<Double>(PARAM_STEEPNESS, &steepness_, "Steepness", "", 1.0);
  parameters_.Bind<string>(PARAM_SSB, &ssb_, "SSB Label (derived quantity)", "");
  parameters_.Bind<string>(PARAM_B0_PHASE, &phase_b0_label_, "Initialisation phase Label that b0 is from", "", "");
  parameters_.Bind<Double>(PARAM_YCS_VALUES, &ycs_values_, "YCS Values", "");
  parameters_.Bind<bool>(PARAM_PRIOR_YCS_VALUES, &prior_ycs_values_, "Priors for year class strength on ycs values (not standardised ycs values)", "",true);
  parameters_.Bind<unsigned>(PARAM_STANDARDISE_YCS_YEARS, &standardise_ycs_, "Years that are included for year class standardisation", "", true);
  parameters_.Bind<string>(PARAM_UNITS, &b0_units_, "Units of B0, if initialising model using B0", "", "");

  RegisterAsEstimable(PARAM_R0, &r0_);
  RegisterAsEstimable(PARAM_B0, &b0_);
  RegisterAsEstimable(PARAM_STEEPNESS, &steepness_);
  RegisterAsEstimable(PARAM_PROPORTIONS, &proportions_);
  RegisterAsEstimable(PARAM_YCS_VALUES, &ycs_values_);

  phase_b0_         = 0;
  process_type_     = ProcessType::kRecruitment;
  partition_structure_ = PartitionStructure::kAge;
}

/**
 *
 */
void RecruitmentBevertonHolt::DoValidate() {
  category_labels_ = model_->categories()->ExpandLabels(category_labels_, parameters_.Get(PARAM_CATEGORIES));

  if (parameters_.Get(PARAM_B0)->has_been_defined() & (b0_units_ == ""))
    LOG_FATAL_P(PARAM_B0) << "Must define unit of B0, for models initialised by B0";
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
}

/**
 * Build the runtime relationships between this object and it's
 */
void RecruitmentBevertonHolt::DoBuild() {
  partition_.Init(category_labels_);

  if (parameters_.Get(PARAM_B0)->has_been_defined())
    bo_initialised_ = true;

  if (phase_b0_label_ != "")
    phase_b0_ = model_->managers().initialisation_phase()->GetPhaseIndex(phase_b0_label_);

  if (parameters_.Get(PARAM_B0)->has_been_defined())
    b0_ = math::convert_units_to_kgs(b0_, b0_units_);

  derived_quantity_ = model_->managers().derived_quantity()->GetDerivedQuantity(ssb_);
  if (!derived_quantity_)
    LOG_ERROR_P(PARAM_SSB) << "(" << ssb_ << ") could not be found. Have you defined it?";

  /**
   * Calculate out SSB offset
   */
  if (!parameters_.Get(PARAM_SSB_OFFSET)->has_been_defined()) {
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
      LOG_WARNING() << "BH recruitment year offset has been calculated on the basis of a single ageing process. We have identified "
          << ageing_processes << " ageing processes we suggest manually setting ssb_offset";
    if (ageing_index == std::numeric_limits<unsigned>::max())
      LOG_ERROR() << location() << " could not calculate the ssb_offset because there is no ageing process";

    if (recruitment_index < ageing_index && ageing_index < derived_quantity_index)
      ssb_offset_ = model_->min_age() + 1;
    else if (derived_quantity_index < ageing_index && ageing_index < recruitment_index)
      ssb_offset_ = model_->min_age() - 1;
    else
      ssb_offset_ = model_->min_age();

    LOG_FINEST() << "SSB Offset calculated to be = " << ssb_offset_ << "; recruitment index = " << recruitment_index << "; ageing index = "
        << ageing_index << "; derived_quantity index = " << derived_quantity_index;

  }


  // validate the standardise YCS year range
  if (standardise_ycs_.size() == 0) {
    for (unsigned year = model_->start_year(); year <= model_->final_year(); ++year)
      standardise_ycs_.push_back(year - ssb_offset_);

  } else if (standardise_ycs_.size() > 1) {
    for (unsigned i = 1; i < standardise_ycs_.size(); ++i) {
      if (standardise_ycs_[i - 1] >= standardise_ycs_[i])
        LOG_ERROR_P(PARAM_STANDARDISE_YCS_YEARS) << " values must be in numeric ascending order. Value "
            << standardise_ycs_[i - 1] << " is not less than " << standardise_ycs_[i];

    }
  }

  if (standardise_ycs_[0] < model_->start_year() - ssb_offset_)
    LOG_ERROR_P(PARAM_STANDARDISE_YCS_YEARS) << " first value is less than the model's start_year";
  if ((*standardise_ycs_.rbegin()) > model_->final_year() - ssb_offset_)
    LOG_ERROR_P(PARAM_STANDARDISE_YCS_YEARS) << " final value (" << (*standardise_ycs_.rbegin())
        << ") is greater than the model's final year - ssb_offset (" << model_->final_year() - ssb_offset_ << ")";

  // check the number of ycs_values_ supplied matches number of years
  unsigned number_of_years = model_->final_year() - model_->start_year() + 1;
  if (ycs_values_.size() != number_of_years)
    LOG_ERROR_P(PARAM_YCS_VALUES) << " need to be defined for every year. Expected " << number_of_years << " but got " << ycs_values_.size();

  for (Double value : ycs_values_) {
    if (value < 0.0)
      LOG_ERROR_P(PARAM_YCS_VALUES) << " value " << value << " cannot be less than 0.0";
  }

  DoReset();
}

/**
 * Reset all of the values so they're ready for an execution run
 */
void RecruitmentBevertonHolt::DoReset() {
  LOG_TRACE();

  ssb_values_.clear();
  ycs_years_.clear();
  true_ycs_values_.clear();
  recruitment_values_.clear();

  for (unsigned i = model_->start_year(); i <= model_->final_year(); ++i)
    ycs_years_.push_back(i - ssb_offset_);

  Double mean_ycs = 0;
  for (unsigned i = 0; i < ycs_years_.size(); ++i) {
    for (unsigned j = 0; j < standardise_ycs_.size(); ++j) {
      if (ycs_years_[i] == standardise_ycs_[j]) {
        mean_ycs += ycs_values_[i];
        break;
      }
    }
  }
  stand_ycs_values_ = ycs_values_;
  mean_ycs /= standardise_ycs_.size();
  for (unsigned i = 0; i < ycs_years_.size(); ++i) {
    for (unsigned j = 0; j < standardise_ycs_.size(); ++j) {
      if (ycs_years_[i] == standardise_ycs_[j])
        stand_ycs_values_[i] = ycs_values_[i] / mean_ycs;
    }
  }
}

/**
 * Execute this process.
 */

void RecruitmentBevertonHolt::DoExecute() {
  Double amount_per = 0.0;
  if (prior_ycs_values_)
    ycs_values_ = stand_ycs_values_;

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
    LOG_FINEST() << "standardise_ycs_.size(): " << standardise_ycs_.size() << "; model_->current_year(): " << model_->current_year()
        << "; model_->start_year(): " << model_->start_year();
    Double ycs;
    // If projection mode ycs values get replaced with projected value from @project block
    if (RunMode::kProjection && model_->current_year() > model_->final_year()) {
      ycs = ycs_values_[model_->current_year() - model_->start_year()];
      LOG_FINEST() << "Size of ycs values: " << ycs_values_.size() << " should be one more entry from previous year";
    } else
      ycs = stand_ycs_values_[model_->current_year() - model_->start_year()];

    if (!parameters_.Get(PARAM_B0)->has_been_defined())
      b0_ = derived_quantity_->GetLastValueFromInitialisation(phase_b0_);
    unsigned ssb_year = model_->current_year() - ssb_offset_;
    Double SSB;
    if (ssb_year < model_->start_year()) {
      initialisationphases::Manager& init_phase_manager = *model_->managers().initialisation_phase();
      LOG_FINE() << "Initialisation phase index SSB is being extracted from init phase " << init_phase_manager.last_executed_phase()
          << " SSB year = " << ssb_year;
      SSB = derived_quantity_->GetLastValueFromInitialisation(init_phase_manager.last_executed_phase());
    } else {
      SSB = derived_quantity_->GetValue(ssb_year);
    }
    Double ssb_ratio = SSB / b0_;
    Double SR = ssb_ratio / (1.0 - ((5.0 * steepness_ - 1.0) / (4.0 * steepness_)) * (1.0 - ssb_ratio));
    Double true_ycs = ycs * SR;
    amount_per = r0_ * true_ycs;

    true_ycs_values_.push_back(true_ycs);
    recruitment_values_.push_back(amount_per);
    ssb_values_.push_back(derived_quantity_->GetValue(ssb_year));

    LOG_FINEST() << "year = " << model_->current_year() << " SSB= " << SSB << " SR = " << SR << "; ycs = "
        << ycs_values_[model_->current_year() - model_->start_year()] << " Standardised year class = "
        << stand_ycs_values_[model_->current_year() - model_->start_year()] << "; b0_ = " << b0_ << "; ssb_ratio = " << ssb_ratio << "; true_ycs = "
        << true_ycs << "; amount_per = " << amount_per;
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
void RecruitmentBevertonHolt::ScalePartition() {
  if (!parameters_.Get(PARAM_B0)->has_been_defined())
    LOG_CODE_ERROR() << "Cannot apply this method as B0 has not been defined";

  have_scaled_partition = true;
  Double SSB = derived_quantity_->GetValue(model_->start_year() - ssb_offset_);
  LOG_MEDIUM() << "Last SSB value = " << SSB;
  Double scalar = b0_ / SSB;
  LOG_MEDIUM() << "Scalar = " << scalar << " B0 = " << b0_;
  LOG_FINEST() << "r0_ value = " << r0_;
  r0_ = 1 * scalar;
  for (auto category : partition_) {
    for (unsigned j = 0; j < category->data_.size(); ++j) {
      LOG_MEDIUM() << "Category "<< category->name_ << " Age = " << j + category->min_age_ << " Numbers at age = " <<  category->data_[j];
      category->data_[j] *= scalar;
      LOG_MEDIUM() << "Category "<< category->name_ << " Age = " << j + category->min_age_ << " Numbers at age = " <<  category->data_[j];
    }
  }
  LOG_MEDIUM() << "R0 = " << r0_;
}


} /* namespace processes */
} /* namespace niwa */
