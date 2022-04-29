/**
 * @file GrowthProcessExists.cpp
 * @author C.Marsh
 * @brief This file will verify an GrowthProcessExists for length based models
 * @version 0.1
 * @date 2021-11-09
 *
 * @copyright Copyright (c) 2022 - NIWA
 *
 */

// headers
#include "../../Model/Model.h"
#include "../Manager.h"
#include "../Process.h"

// namespaces
namespace niwa::processes::verification {

/**
 * @brief Check to see if at least one ageing process has been defined on the model
 *
 * @param model Model pointer
 */
void GrowthProcessExists(shared_ptr<Model> model) {
  auto process_list = model->managers()->process()->objects();
  for (auto* process : process_list) {
    if (process->process_type() == ProcessType::kGrowth)
      return;
  }
  LOG_VERIFY() << "No Growth process has been defined in the model";
}

}  // namespace niwa::processes::verification