/**
 * @file RecruitmentConstantNoDupeCategories.cpp
 * @author Scott Rasmussen (scott@zaita.com)
 * @brief Ensure we're not recruiting into duplicate categories
 * @version 0.1
 * @date 2021-11-09
 *
 * @copyright Copyright (c) 2021 - NIWA
 *
 */

// headers
#include "../../Model/Model.h"
#include "../Age/RecruitmentBevertonHolt.h"
#include "../Age/RecruitmentConstant.h"
#include "../Manager.h"
#include "../Process.h"

// namespaces
namespace niwa::processes::verification {

/**
 * @brief Check to see if at least one ageing process has been defined on the model
 *
 * @param model Model pointer
 */
void RecruitmentNoDupelicateCategories(shared_ptr<Model> model) {
  map<string, unsigned> category_count;

  auto process_list = model->managers()->process()->objects();
  for (auto* process : process_list) {
    if (process->process_type() == ProcessType::kRecruitment) {
      if (process->type() == PARAM_RECRUITMENT_CONSTANT) {
        age::RecruitmentConstant* recruitment = dynamic_cast<age::RecruitmentConstant*>(process);
        if (!recruitment)
          LOG_CODE_ERROR() << "!rec with auto* rec = dynamic_cast<age::RecruitmentConstant*>(process)";

        for (auto label : recruitment->category_labels()) category_count[label]++;

      } else if (process->type() == PARAM_RECRUITMENT_BEVERTON_HOLT) {
        age::RecruitmentBevertonHolt* recruitment = dynamic_cast<age::RecruitmentBevertonHolt*>(process);
        if (!recruitment)
          LOG_CODE_ERROR() << "!rec with auto* rec = dynamic_cast<age::RecruitmentBevertonHolt*>(process)";

        for (auto label : recruitment->category_labels()) category_count[label]++;
      }
    }
  }

  // check to ensure we only have 1 of each category
  for (auto iter : category_count)
    if (iter.second > 1)
      LOG_VERIFY() << "Category " << iter.first << " is present in " << iter.second << " recruitment processes";
}

}  // namespace niwa::processes::verification