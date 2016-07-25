/**
 * @file MortalityConstantRate.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "MortalityConstantRate.h"

#include <numeric>

#include "Categories/Categories.h"
#include "Selectivities/Manager.h"
#include "Selectivities/Selectivity.h"
#include "TimeSteps/Manager.h"

// Namespaces
namespace niwa {
namespace processes {

/**
 * Default Constructor
 */
MortalityConstantRate::MortalityConstantRate(Model* model)
  : Process(model),
    partition_(model) {
  LOG_TRACE();
  process_type_ = ProcessType::kMortality;
  partition_structure_ = PartitionStructure::kAge;

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "List of categories", "");
  parameters_.Bind<Double>(PARAM_M, &m_input_, "Mortality rates", "");
  parameters_.Bind<Double>(PARAM_TIME_STEP_RATIO, &ratios_, "Time step ratios for M", "", true);
  parameters_.Bind<string>(PARAM_SELECTIVITIES, &selectivity_names_, "Selectivities", "");

  RegisterAsEstimable(PARAM_M, &m_);
}

/**
 * Validate our Mortality Constant Rate process
 *
 * - Validate the required parameters
 * - Assign the label from the parameters
 * - Assign and validate remaining parameters
 * - Duplicate 'm' and 'selectivities' if only 1 vale specified
 * - Check m is between 0.0 and 1.0
 * - Check the categories are real
 */
void MortalityConstantRate::DoValidate() {
  category_labels_ = model_->categories()->ExpandLabels(category_labels_, parameters_.Get(PARAM_CATEGORIES));

  if (m_input_.size() == 1)
    m_input_.assign(category_labels_.size(), m_input_[0]);
  if (selectivity_names_.size() == 1)
    selectivity_names_.assign(category_labels_.size(), selectivity_names_[0]);

  if (m_input_.size() != category_labels_.size()) {
    LOG_ERROR_P(PARAM_M)
        << ": Number of Ms provided is not the same as the number of categories provided. Expected: "
        << category_labels_.size()<< " but got " << m_input_.size();
  }

  if (selectivity_names_.size() != category_labels_.size()) {
    LOG_ERROR_P(PARAM_SELECTIVITIES)
        << ": Number of selectivities provided is not the same as the number of categories provided. Expected: "
        << category_labels_.size()<< " but got " << selectivity_names_.size();
  }

  // Validate our Ms are between 1.0 and 0.0
  for (Double m : m_input_) {
    if (m < 0.0 || m > 1.0)
      LOG_ERROR_P(PARAM_M) << ": m value " << AS_DOUBLE(m) << " must be between 0.0 and 1.0 (inclusive)";
  }

  for (unsigned i = 0; i < m_input_.size(); ++i)
    m_[category_labels_[i]] = m_input_[i];

  // Check categories are real
  for (const string& label : category_labels_) {
    if (!model_->categories()->IsValid(label))
      LOG_ERROR_P(PARAM_CATEGORIES) << ": category " << label << " does not exist. Have you defined it?";
  }
}

/**
 * Build any runtime relationships
 * - Build the partition accessor
 * - Build our list of selectivities
 * - Build our ratios for the number of time steps
 */
void MortalityConstantRate::DoBuild() {
  partition_.Init(category_labels_);

  for (string label : selectivity_names_) {
    Selectivity* selectivity = model_->managers().selectivity()->GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR_P(PARAM_SELECTIVITIES) << ": selectivity " << label << " does not exist. Have you defined it?";

    selectivities_.push_back(selectivity);
  }

  /**
   * Organise our time step ratios. Each time step can
   * apply a different ratio of M so here we want to verify
   * we have enough and re-scale them to 1.0
   */
  vector<TimeStep*> time_steps = model_->managers().time_step()->ordered_time_steps();
  LOG_FINEST() << "time_steps.size(): " << time_steps.size();
  vector<unsigned> active_time_steps;
  for (unsigned i = 0; i < time_steps.size(); ++i) {
    if (time_steps[i]->HasProcess(label_))
      active_time_steps.push_back(i);
  }

  if (ratios_.size() == 0) {
    for (unsigned i : active_time_steps)
      time_step_ratios_[i] = 1.0;
  } else {
    if (ratios_.size() != active_time_steps.size())
      LOG_ERROR_P(PARAM_TIME_STEP_RATIO) << " length (" << ratios_.size()
          << ") does not match the number of time steps this process has been assigned to (" << active_time_steps.size() << ")";

    for (Double value : ratios_) {
      if (value < 0.0 || value > 1.0)
        LOG_ERROR_P(PARAM_TIME_STEP_RATIO) << " value (" << value << ") must be between 0.0 (exclusive) and 1.0 (inclusive)";
    }

    for (unsigned i = 0; i < ratios_.size(); ++i)
      time_step_ratios_[active_time_steps[i]] = ratios_[i];
  }
}

/**
 * Execute the process
 */
void MortalityConstantRate::DoExecute() {
  LOG_FINEST() << "year: " << model_->current_year();

  // get the ratio to apply first
  unsigned time_step = model_->managers().time_step()->current_time_step();

  LOG_FINEST() << "Ratios.size() " << time_step_ratios_.size() << " : time_step: " << time_step << "; ratio: " << time_step_ratios_[time_step];
  Double ratio = time_step_ratios_[time_step];

  StoreForReport("year", model_->current_year());

  unsigned i = 0;
  for (auto category : partition_) {
    Double m = m_[category->name_];

    unsigned j = 0;
    LOG_FINEST() << "category " << category->name_ << "; min_age: " << category->min_age_ << "; ratio: " << ratio;
    StoreForReport(category->name_ + " ratio", ratio);
    for (Double& data : category->data_) {
      data -= data * (1-exp(-selectivities_[i]->GetResult(category->min_age_ + j, category->age_length_)  * (m * ratio)));
      ++j;
    }

    ++i;
  }
}

/**
 * Reset the Mortality Process
 */
void MortalityConstantRate::DoReset() {
  mortality_rates_.clear();
}

} /* namespace processes */
} /* namespace niwa */
