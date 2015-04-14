/**
 * @file ProcessProportionsByCategory.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "ProcessProportionsByCategory.h"

#include "Processes/Manager.h"
#include "Utilities/DoubleCompare.h"

// namespaces
namespace niwa {
namespace observations {

/**
 * Default constructor
 */
ProcessProportionsByCategory::ProcessProportionsByCategory() {
  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "Process label", "");
  parameters_.Bind<Double>(PARAM_PROCESS_PROPORTION, &process_proportion_, "Process proportion", "", Double(0.5));

  mean_proportion_method_ = false;
}

/**
 * Build our process proportions by category
 */
void ProcessProportionsByCategory::DoBuild() {
  ProportionsByCategory::DoBuild();

  if (process_proportion_ < 0.0 || process_proportion_ > 1.0)
    LOG_ERROR_P(PARAM_PROCESS_PROPORTION) << ": process_proportion (" << AS_DOUBLE(process_proportion_) << ") must be between 0.0 and 1.0";
  proportion_of_time_ = process_proportion_;

  ProcessPtr process = processes::Manager::Instance().GetProcess(process_label_);
  if (!process)
    LOG_ERROR_P(PARAM_PROCESS) << " " << process_label_ << " could not be found. Have you defined it?";

  for (unsigned year : years_)
    process->Subscribe(year, time_step_label_, shared_ptr());
}

} /* namespace observations */
} /* namespace niwa */
