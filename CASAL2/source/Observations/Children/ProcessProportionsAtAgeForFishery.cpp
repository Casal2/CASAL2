/**
 * @file ProcessProportionsAtAgeForFishery.cpp
 * @author C Marsh
 * @github https://github.com/Zaita
 * @date 25/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "ProcessProportionsAtAgeForFishery.h"

#include <memory>

#include "TimeSteps/Manager.h"
#include "Utilities/DoubleCompare.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
ProcessProportionsAtAgeForFishery::ProcessProportionsAtAgeForFishery(Model* model)
  : observations::ProportionsAtAgeForFishery(model) {

  parameters_.Bind<string>(PARAM_PROCESS, &process_label_, "Process label", "");
  mean_proportion_method_ = false;
}

/**
 *
 */
void ProcessProportionsAtAgeForFishery::DoBuild() {
  ProportionsAtAgeForFishery::DoBuild();

  for (string time_label : time_step_label_) {
    auto time_step = model_->managers().time_step()->GetTimeStep(time_label);
    if (!time_step) {
      LOG_FATAL_P(PARAM_TIME_STEP) << time_label << " could not be found. Have you defined it?";
    } else {
      auto process = time_step->SubscribeToProcess(this, years_, process_label_);
      mortality_instantaneous_ = dynamic_cast<MortalityInstantaneous*>(process);
    }
  }

  if (!mortality_instantaneous_)
    LOG_ERROR_P(PARAM_PROCESS) << "This observation can only be used for Process of type = " << PARAM_MORTALITY_INSTANTANEOUS;
}

} /* namespace observations */
} /* namespace niwa */
