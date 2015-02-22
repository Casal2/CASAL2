/**
 * @file AgeLength.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 23/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "AgeLength.h"

#include "TimeSteps/Manager.h"
#include "Utilities/Map.h"

// namespaces
namespace niwa {

/**
 *
 */
AgeLength::AgeLength() {
  parameters_.Bind<string>(PARAM_LABEL, &label_, "Label", "");
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type", "");
  parameters_.Bind<Double>(PARAM_TIME_STEP_PROPORTIONS, &time_step_proportions_, "", "", true);
}

/**
 *
 */
void AgeLength::Validate() {
  parameters_.Populate();

  DoValidate();
}

/**
 *
 */
void AgeLength::Build() {
  unsigned time_step_count = timesteps::Manager::Instance().time_steps().size();
  if (time_step_proportions_.size() == 0) {
    time_step_proportions_.assign(time_step_count, 0.0);
  } else if (time_step_count != time_step_proportions_.size()) {
    LOG_ERROR(parameters_.location(PARAM_TIME_STEP_PROPORTIONS) << " size (" << time_step_proportions_.size() << "( must match the number "
        "of defined time steps for this process (" << time_step_count << ")");
  }

  for (auto iter : time_step_proportions_) {
    if (iter < 0.0 || iter > 1.0)
      LOG_ERROR(parameters_.location(PARAM_TIME_STEP_PROPORTIONS) << " value (" << iter << ") must be in the range 0.0-1.0");
  }

  DoBuild();
}

} /* namespace niwa */
