/**
 * @file MortalityConstantRate.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "MortalityConstantRate.h"

#include <numeric>

#include "Categories/Categories.h"
#include "Selectivities/Manager.h"
#include "Utilities/Logging/Logging.h"

// Namespaces
namespace isam {
namespace processes {

/**
 * Default Constructor
 */
MortalityConstantRate::MortalityConstantRate()
: time_steps_manager_(timesteps::Manager::Instance()) {

  LOG_TRACE();

  model_ = Model::Instance();
  is_mortality_process = true;

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_names_, "List of categories", "");
  parameters_.Bind<double>(PARAM_M, &m_, "Mortality rates", "");
  parameters_.Bind<double>(PARAM_TIME_STEP_RATIO, &ratios_, "Time step ratios for M", "", true);
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
  if (m_.size() == 1)
    m_.assign(category_names_.size(), m_[0]);
  if (selectivity_names_.size() == 1)
    selectivity_names_.assign(category_names_.size(), selectivity_names_[0]);

  if (m_.size() != category_names_.size()) {
    LOG_ERROR(parameters_.location(PARAM_M)
        << ": Number of Ms provided is not the same as the number of categories provided. Expected: "
        << category_names_.size()<< " but got " << m_.size());
  }

  if (selectivity_names_.size() != category_names_.size()) {
    LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES)
        << ": Number of selectivities provided is not the same as the number of categories provided. Expected: "
        << category_names_.size()<< " but got " << selectivity_names_.size());
  }

  // Validate our Ms are between 1.0 and 0.0
  for (Double m : m_) {
    if (m < 0.0 || m > 1.0)
      LOG_ERROR(parameters_.location(PARAM_M) << ": m value " << AS_DOUBLE(m) << " must be between 0.0 and 1.0 (inclusive)");
  }

  // Check categories are real
  for (const string& label : category_names_) {
    if (!Categories::Instance()->IsValid(label))
      LOG_ERROR(parameters_.location(PARAM_CATEGORIES) << ": category " << label << " does not exist. Have you defined it?");
  }
}

/**
 * Build any runtime relationships
 * - Build the partition accessor
 * - Build our list of selectivities
 * - Build our ratios for the number of time steps
 */
void MortalityConstantRate::DoBuild() {
  partition_.Init(category_names_);

  for (string label : selectivity_names_) {
    SelectivityPtr selectivity = selectivities::Manager::Instance().GetSelectivity(label);
    if (!selectivity)
      LOG_ERROR(parameters_.location(PARAM_SELECTIVITIES) << ": selectivity " << label << " does not exist. Have you defined it?");

    selectivities_.push_back(selectivity);
  }

  /**
   * Organise our time step ratios. Each time step can
   * apply a different ratio of M so here we want to verify
   * we have enough and re-scale them to 1.0
   */
  vector<TimeStepPtr> time_steps = time_steps_manager_.time_steps();
  LOG_INFO("time_steps.size(): " << time_steps.size());
  unsigned time_step_count = 0;
  for(TimeStepPtr time_step : time_steps) {
    if (time_step->HasProcess(label_))
      ++time_step_count;
  }
  LOG_INFO("ratios_.size(): " << ratios_.size() << " | time_step_count: " << time_step_count);
  if (ratios_.size() == 0)
    ratios_.assign(time_step_count, 1.0);
  else {
    if (ratios_.size() != time_step_count)
      LOG_ERROR(parameters_.location(PARAM_TIME_STEP_RATIO) << " length (" << ratios_.size()
          << ") does not match the number of time steps this process has been assigned too (" << time_step_count << ")");

    Double sum = std::accumulate(ratios_.begin(), ratios_.end(), 0);
    for (Double &value : ratios_)
      value = value / sum;
  }
}

/**
 * Execute the process
 */
void MortalityConstantRate::Execute() {
  LOG_TRACE();

  // get the ratio to apply first
  unsigned time_step = time_steps_manager_.current_time_step();

  LOG_INFO("Ratios.size() " << ratios_.size() << " : time_step: " << time_step);
  Double ratio = model_->state() == State::kInitialise ? 1.0 : ratios_[time_step];

  unsigned i = 0;
  for (auto category : partition_) {
    Double m = m_.size() > 1 ? m_[i] : m_[0];

    unsigned j = 0;
    for (Double& data : category->data_) {
      data -= data * (1-exp(-selectivities_[i]->GetResult(category->min_age_ + j)  * (m * ratio)));
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
} /* namespace isam */
