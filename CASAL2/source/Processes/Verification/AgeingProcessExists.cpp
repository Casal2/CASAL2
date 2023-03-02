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
#include <iostream>
#include <sstream>

#include "../../Categories/Categories.h"
#include "../../Model/Model.h"
#include "../Age/Ageing.h"
#include "../Manager.h"
#include "../Process.h"

// namespaces
namespace niwa::processes::verification {

/**
 * @brief Check to see if at least one ageing process has been defined on the model, and if every category has been aged
 * @param model Model pointer
 */
void AgeingProcessExists(shared_ptr<Model> model) {
  auto           process_list = model->managers()->process()->objects();
  vector<string> missing_categories;
  bool           model_has_ageing = false;

  for (auto* process : process_list) {
    if (process->process_type() == ProcessType::kAgeing) {
      model_has_ageing              = true;
      age::Ageing*   AgeProcess     = dynamic_cast<age::Ageing*>(process);
      vector<string> all_categories = model->categories()->category_names();
      for (auto label : all_categories) {
        if (find(AgeProcess->category_labels().begin(), AgeProcess->category_labels().end(), label) == AgeProcess->category_labels().end())
          missing_categories.push_back(label);
      }
    }
  }
  if (missing_categories.size() > 0) {
    LOG_WARNING() << "The following categories were not included in any ageing processes: " << utilities::String::join(missing_categories, ", ")
                  << ". Please check your input configuration file";
  }

  if (!model_has_ageing) {
    LOG_VERIFY() << "This is an age based model, but no ageing processes have been defined";
  }
}

}  // namespace niwa::processes::verification
