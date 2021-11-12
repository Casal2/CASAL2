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
#include "../../Categories/Categories.h"
#include "../../InitialisationPhases/Manager.h"
#include "../../Model/Model.h"
#include "../Age/RecruitmentBevertonHolt.h"
#include "../Age/RecruitmentBevertonHoltWithDeviations.h"
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
void RecruitmentCategoriesVerification(shared_ptr<Model> model) {
  map<string, unsigned> category_count;
  map<string, unsigned> category_in_recruitment_that_scale;
  vector<string>        all_categories = model->categories()->category_names();

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

        for (auto label : recruitment->category_labels()) {
          category_in_recruitment_that_scale[label]++;
          category_count[label]++;
        }
      } else if (process->type() == PARAM_RECRUITMENT_BEVERTON_HOLT_WITH_DEVIATIONS) {
        age::RecruitmentBevertonHoltWithDeviations* recruitment = dynamic_cast<age::RecruitmentBevertonHoltWithDeviations*>(process);
        if (!recruitment)
          LOG_CODE_ERROR() << "!rec with auto* rec = dynamic_cast<age::RecruitmentBevertonHolt*>(process)";

        for (auto label : recruitment->category_labels()) {
          category_in_recruitment_that_scale[label]++;
          category_count[label]++;
        }
      }
    }
  }

  // check to ensure we only have 1 of each category
  for (auto iter : category_count)
    if (iter.second > 1)
      LOG_VERIFY() << "Category " << iter.first << " is present in " << iter.second << " recruitment processes";

  // If we have an initialisation of type derived do a check that categories are there, otherwise can cause an issue with scaling
  // only a problem if, B0 initialised tho.
  auto init_list                        = model->managers()->initialisation_phase()->objects();
  bool there_is_type_derived_init_phase = false;
  for (auto* init : init_list) {
    if (init->type() == PARAM_DERIVED)
      there_is_type_derived_init_phase = true;
  }
  // check all categories in the system have been assigned to a recruitment block.
  // For categories that are not present during initialisation i.e. tagged fish, you don't need
  // to specify them in a recruitment block. For this case you can run with casal2 -V warning
  if (there_is_type_derived_init_phase) {
    for (auto state_category : all_categories) {
      unsigned recruit_category_counter = 0;
      for (auto iter : category_in_recruitment_that_scale) {
        ++recruit_category_counter;
        if (iter.first == state_category) {
          break;
        }
        LOG_FINE() << "categories = " << category_count.size() << " counted = " << recruit_category_counter;
        /* This may not be useful - commented out for the time being
               if (recruit_category_counter >= category_count.size()) {
                 LOG_WARNING()
                     << "The category " << state_category
                     << " is not used in @process[type=recruitment]. This may be expected for models that have categories that are not used at the beginning of the model e.g.
           tagged categories.";
        }
       */
      }
    }
  }
}

}  // namespace niwa::processes::verification