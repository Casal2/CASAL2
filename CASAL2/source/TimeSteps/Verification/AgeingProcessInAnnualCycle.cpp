/**
 * @file AgeingProcessInAnnualCycle.cpp
 * @author C. Marsh
 * @brief Ageing in the annualcycle
 * @version 0.1
 * @date 2022
 *
 * @copyright Copyright (c) 2022 - NIWA
 *
 */

// headers
#include "../../Model/Model.h"
#include "../../Processes/Process.h"
#include "../Manager.h"
#include "../TimeStep.h"

// namespaces
namespace niwa::timesteps::verification {

/**
 * @brief Check to see if there is an ageing process in the annual cycle
 *
 * @param model Model pointer
 */

void AgeingProcessInAnnualCycle(shared_ptr<Model> model) {
  const vector<TimeStep*> ordered_time_steps               = model->managers()->time_step()->ordered_time_steps();
  // loop through time steps
  unsigned ageing_processes = 0;
  for (auto time_step : ordered_time_steps) {
    for (auto process : time_step->processes()) {
      if (process->process_type() == ProcessType::kAgeing) {
        ageing_processes++;
      }
    }
  }
  if(ageing_processes == 0) {
    LOG_VERIFY() << "No ageing process has been defined in the annual cycle.";
  }
}

}  // namespace niwa::processes::verification