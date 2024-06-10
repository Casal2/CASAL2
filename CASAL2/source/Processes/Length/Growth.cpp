/**
 * @file Growth.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 27/07/2017
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// Headers
#include "Growth.h"

#include "../../Categories/Categories.h"
#include "../../TimeSteps/Manager.h"
#include "../../Utilities/Math.h"
#include "../../Utilities/To.h"
#include "GrowthIncrements/GrowthIncrement.h"

// Namespaces
namespace niwa {
namespace processes {
namespace length {
namespace math = niwa::utilities::math;

/**
 * Default constructor
 */
Growth::Growth(shared_ptr<Model> model) : Process(model), partition_(model) {
  process_type_        = ProcessType::kGrowth;
  partition_structure_ = PartitionType::kLength;
  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The labels of the categories", "");
}

/**
 * Validate our Growth Process process.
 *
 * 1. Check for any required parameters
 * 2. Assign the label from our parameters
 * 3. Check categories
 */
void Growth::DoValidate() {}

/**
 * Build objects that are needed by this object during the execution phase. This
 * includes things like the partition accessor it will need.
 *
 * Then build values that we want to print when print is called.
 *
 */
void Growth::DoBuild() {
  partition_.Init(category_labels_);
  new_length_partition_.resize(model_->get_number_of_length_bins(), 0.0);
}

void Growth::DoReset() {}

/**
 * Execute our length growth class.
 */
void Growth::DoExecute() {
  LOG_TRACE();
  LOG_FINE() << "Calculate growth in time_step: " << model_->managers()->time_step()->current_time_step() << " in year: " << model_->current_year();
  for (auto& category : partition_) {
    // reset container
    LOG_FINEST() << "Calculate growth for category: " << category->name_;
    fill(new_length_partition_.begin(), new_length_partition_.end(), 0.0);
    transition_matrix_ = category->growth_increment_->get_transition_matrix();
    for (unsigned i = 0; i < model_->get_number_of_length_bins(); i++) {
      for (unsigned j = 0; j < model_->get_number_of_length_bins(); j++) {
        new_length_partition_[j] += transition_matrix_[i][j] * category->data_[i];
      }
    }
    // now copy it over;
    // for (unsigned i = 0; i < model_->get_number_of_length_bins(); i++) {
    //   LOG_FINE() << "i = " << i << " previous value = " << category->data_[i] << " new value = " << new_length_partition_[i];
    // }
    category->data_ = new_length_partition_;
    // for (unsigned i = 0; i < model_->get_number_of_length_bins(); i++) {
    // LOG_FINE() << "i = " << i << " updated value = " << category->data_[i];
    // }
  }
}

} /* namespace length */
} /* namespace processes */
} /* namespace niwa */
