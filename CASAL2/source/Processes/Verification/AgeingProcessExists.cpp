/**
 * @file AgeingProcessExists.cpp
 * @author Scott Rasmussen (scott@zaita.com)
 * @brief This file will verify an ageing process exists in the model
 * @version 0.1
 * @date 2021-11-09
 *
 * @copyright Copyright (c) 2021 - NIWA
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
void AgeingProcessExists(shared_ptr<Model> model) {
  auto process_list = model->managers()->process()->objects();
  for (auto* process : process_list) {
    if (process->process_type() == ProcessType::kAgeing)
      return;
  }

  LOG_VERIFY() << "No ageing process has been defined in the model";
}

}  // namespace niwa::processes::verification