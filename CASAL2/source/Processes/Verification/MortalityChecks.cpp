/**
 * @file MortalityChecks.cpp
 * @author C. Marsh
 * @brief Ensure M is on all categories
 * @version 0.1
 * @date 2021-11-09
 *
 * @copyright Copyright (c) 2021 - NIWA
 *
 */

// headers
#include "../Age/Mortality/MortalityHybrid.h"
#include "../Age/Mortality/MortalityInstantaneousRetained.h"
#include "../Common/Mortality/MortalityConstantRemovalRate.h"
#include "../Common/Mortality/MortalityInstantaneous.h"
#include "../Manager.h"
#include "../Process.h"
#include "Categories/Categories.h"
#include "InitialisationPhases/Manager.h"
#include "Model/Model.h"

// namespaces
namespace niwa::processes::verification {

/**
 * @brief Check to see if all categories have an assigned M value.
 *
 * @param model Model pointer
 */
void AllCategoriesHaveAnM(shared_ptr<Model> model) {
  map<string, unsigned> category_count;
  vector<string>        all_categories = model->categories()->category_names();
  if (model->partition_type() == PartitionType::kAge) {
    auto process_list = model->managers()->process()->objects();
    for (auto* process : process_list) {
      if (process->process_type() == ProcessType::kMortality) {
        if (process->type() == PARAM_MORTALITY_CONSTANT_RATE || process->type() == PARAM_MORTALITY_CONSTANT_EXPLOITATION) {
          common::MortalityConstantRemovalRate* mortality = dynamic_cast<common::MortalityConstantRemovalRate*>(process);
          if (!mortality)
            LOG_CODE_ERROR() << "!mortality with auto* mortality = dynamic_cast<common::MortalityConstantRemovalRate*>(process)";

          for (auto label : mortality->category_labels()) category_count[label]++;

        } else if (process->type() == PARAM_MORTALITY_INSTANTANEOUS) {
          common::MortalityInstantaneous* mortality = dynamic_cast<common::MortalityInstantaneous*>(process);
          if (!mortality)
            LOG_CODE_ERROR() << "!mortality with auto* mortality = dynamic_cast<common::MortalityInstantaneous*>(process)";

          for (auto label : mortality->category_labels()) {
            category_count[label]++;
          }
        } else if (process->type() == PARAM_MORTALITY_INSTANTANEOUS_RETAINED) {
          age::MortalityInstantaneousRetained* mortality = dynamic_cast<age::MortalityInstantaneousRetained*>(process);
          if (!mortality)
            LOG_CODE_ERROR() << "!mortality with auto* mortality = dynamic_cast<age::MortalityInstantaneousRetained*>(process)";

          for (auto label : mortality->category_labels()) {
            category_count[label]++;
          }
        } else if (process->type() == PARAM_MORTALITY_HYBRID) {
          age::MortalityHybrid* mortality = dynamic_cast<age::MortalityHybrid*>(process);
          if (!mortality)
            LOG_CODE_ERROR() << "!mortality with auto* mortality = dynamic_cast<age::MortalityHybrid*>(process)";

          for (auto label : mortality->category_labels()) {
            category_count[label]++;
          }
        }
      }
    }
  } else if (model->partition_type() == PartitionType::kLength) {
    auto process_list = model->managers()->process()->objects();
    for (auto* process : process_list) {
      if (process->process_type() == ProcessType::kMortality) {
        if (process->type() == PARAM_MORTALITY_CONSTANT_RATE || process->type() == PARAM_MORTALITY_CONSTANT_EXPLOITATION) {
          common::MortalityConstantRemovalRate* mortality = dynamic_cast<common::MortalityConstantRemovalRate*>(process);
          if (!mortality)
            LOG_CODE_ERROR() << "!mortality with auto* mortality = dynamic_cast<common::MortalityConstantRemovalRate*>(process)";

          for (auto label : mortality->category_labels()) category_count[label]++;

        } else if (process->type() == PARAM_MORTALITY_INSTANTANEOUS) {
          common::MortalityInstantaneous* mortality = dynamic_cast<common::MortalityInstantaneous*>(process);
          if (!mortality)
            LOG_CODE_ERROR() << "!mortality with auto* mortality = dynamic_cast<common::MortalityInstantaneous*>(process)";

          for (auto label : mortality->category_labels()) {
            category_count[label]++;
          }
        }
      }
    }
  }

  // check to ensure we only have 1 of each category
  for (auto iter : all_categories) {
    if (category_count[iter] <= 0) {
      LOG_VERIFY() << "The category " << iter
                   << " was not found in any mortality processes. This suggests that there is no mortality processes occuring on this category. This is likely to be an error";
    }
  }
}

}  // namespace niwa::processes::verification
