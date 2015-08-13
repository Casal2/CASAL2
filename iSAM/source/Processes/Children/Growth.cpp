/**
 * @file Growth.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 24/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Growth.h"

// namespaces
namespace niwa {
namespace processes {

/**
 * Default constructor
 */
Growth::Growth() : Process(Model::Instance()) {
  LOG_TRACE();

//  parameters_.Bind<string>(PARAM_CATEGORIES, &category_names_, "Categories", "");
//  parameters_.Bind<Double>(PARAM_TIME_STEP_PROPORTIONS, &time_step_proportions_, "Proportion to age per time step", "", true);

  process_type_ = ProcessType::kAgeing;
  partition_structure_ = PartitionStructure::kLength;
}

/**
 * Validate our Growth process.
 *
 * 1. Check for any required parameters
 * 2. Assign the label from our parameters
 * 3. Assign any remaining parameters
 */
void Growth::DoValidate() {
//  // Ensure defined categories were valid
//  for(const string& category : category_names_) {
//    if (!Categories::Instance()->IsValid(category))
//      LOG_ERROR_P(parameters_.location(PARAM_CATEGORIES) << ": category " << category << " is not a valid category");
//  }
}

/**
 * Build objects that are needed by this object during the execution phase. This
 * includes things like the partition accessor it will need.
 *
 * Then build values that we want to print when print is called.
 *
 * Then build the basics
 */
void Growth::DoBuild() {
//  partition_.Init(category_names_);
//  model_      = Model::Instance();
//
//  if (time_step_proportions_.size() == 0)
//    time_step_proportions_.push_back(1.0);
//
//  vector<unsigned> time_step_indexes = timesteps::Manager::Instance().GetTimeStepIndexesForProcess(label_);
//  if (time_step_indexes.size() != time_step_proportions_.size())
//    LOG_ERROR_P(parameters_.location(PARAM_TIME_STEP_PROPORTIONS) << " size (" << time_step_proportions_.size() << "( must match the number "
//        "of defined time steps for this process (" << time_step_indexes.size() << ")");
//
//  time_step_proportions_by_index_ = utilities::MapCreate(time_step_indexes, time_step_proportions_);
//  for (auto iter : time_step_proportions_by_index_) {
//    if (iter.second < 0.0 || iter.second > 1.0)
//      LOG_ERROR_P(parameters_.location(PARAM_TIME_STEP_PROPORTIONS) << " value (" << iter.second << ") must be in the range 0.0-1.0");
//  }
}

/**
 * Execute our Growth class.
 */
void Growth::DoExecute() {
//  Double amount_to_move = 0.0;
//  Double moved_fish = 0.0;
//
//  unsigned time_step_index = timesteps::Manager::Instance().current_time_step();
//  if (time_step_proportions_by_index_.find(time_step_index) == time_step_proportions_by_index_.end())
//    LOG_CODE_ERROR("time_step_proportions_by_index_.find(" << time_step_index << ") == time_step_proportions_by_index_.end()");
//  Double proportion = time_step_proportions_by_index_[time_step_index];
//
//  for (auto category : partition_) {
//    moved_fish = 0.0;
//    for (Double& data : category->data_) {
//      amount_to_move = data * proportion;
//      data -= amount_to_move;
//      data += moved_fish;
//
//      moved_fish = amount_to_move;
//    }
//
//    if (model_->age_plus())
//      (* category->data_.rbegin() ) += moved_fish;
//  }
}

} /* namespace processes */
} /* namespace niwa */
