/**
 * @file GrowthIncrementConsistentWithGrowthProcess.cpp
 * @author C. Marsh
 * @brief check the growth process is consistent with @growth_increment models
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
#include "../../Processes/Length/Growth.h"
#include "../../Categories/Categories.h"
#include "../../Partition/Category.h"
#include "../../GrowthIncrements/GrowthIncrement.h"

// namespaces
namespace niwa::timesteps::verification {

/**
 * @brief This verifies a few things
 * - all categories in the model are assigned to a growth process
 * - growth_increment time-step-proportions are consistent with growth process
 *
 * @param model Model pointer
 */

void GrowthIncrementConsistentWithGrowthProcess(shared_ptr<Model> model) {
  const vector<TimeStep*> ordered_time_steps               = model->managers()->time_step()->ordered_time_steps();
  // loop through time steps
  map<string, unsigned> category_count;
  map<string, unsigned> category_in_growth;
  map<string, map<unsigned, double>> growth_increment_; // category x time_step_index x proportion
  vector<string>        all_categories = model->categories()->category_names();
  unsigned time_step_ndx = 0;
  for (auto time_step : ordered_time_steps) {
    for (auto process : time_step->processes()) {
      if (process->process_type() == ProcessType::kGrowth) {
        processes::length::Growth* growth = dynamic_cast<processes::length::Growth*>(process);
        if (!growth)
          LOG_CODE_ERROR() << "!growth with auto* growth = dynamic_cast<processes::length::Growth*>(process);";
        // get categories in growth process
        for (auto label : growth->category_labels()) {
          category_in_growth[label]++;
          category_count[label]++;
          auto category = &model->partition().category(label);
          growth_increment_[label][time_step_ndx] = category->growth_increment_->get_time_step_proportions(time_step_ndx);
        }
      }
    }
    time_step_ndx++;
  }
  // check all categories are in at lease one growth process.
  // and that time-step proportions = 1
  for (auto state_category : all_categories) {
    if(!(category_in_growth[state_category] > 0)) {
      LOG_VERIFY() << "The category "  << state_category << " is not included in a growth process";
    }
    double total = 0.0;
    for(auto time_prop : growth_increment_[state_category]) {
      total += time_prop.second;
    }
    if(!utilities::math::IsOne(total)) {
      LOG_VERIFY() << "For the category '"  << state_category << "' the proportion of growth applied = " << total << ". This is the result of the growth process applied in the annual cycle or time_step_proportions of the @growth_increment model. Please check these";
    }
  }
}
}  // namespace niwa::processes::verification