/**
 * @file GrowthBasic.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 27/07/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */

// Headers
#include "GrowthBasic.h"

#include "Utilities/To.h"
#include "Categories/Categories.h"
#include "TimeSteps/Manager.h"
#include "Utilities/Math.h"

// Namespaces
namespace niwa {
namespace processes {
namespace length {
namespace math = niwa::utilities::math;

/**
 * Default constructor
 */
GrowthBasic::GrowthBasic(Model* model)
  : Process(model),
    partition_(model) {
  process_type_ = ProcessType::kTransition;
  partition_structure_ = PartitionType::kLength;

  parameters_.Bind<string>(PARAM_CATEGORIES, &category_labels_, "The labels of the categories", "");
  parameters_.Bind<unsigned>(PARAM_NUMBER_OF_GROWTH_EPISODES, &n_growth_episodes_, "The number of growth episodes per year", "");
  parameters_.Bind<string>(PARAM_GROWTH_TIME_STEPS, &growth_time_steps_, "The time step in which each growth episode occurs", "");
  parameters_.Bind<Double>(PARAM_CV, &cv_ , "The CV for the growth model", "", Double(0.0))->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_SIGMA_MIN, &min_sigma_ , "The lower bound on sigma for the growth model", "", Double(0.0))->set_lower_bound(0.0);

}

/**
 * Validate the Growth Process process.
 *
 * 1. Check for any required parameters
 * 2. Assign the label from the parameters
 * 3. Check categories
 */
void GrowthBasic::DoValidate() {
  if (growth_time_steps_.size() != n_growth_episodes_)
    LOG_ERROR_P(PARAM_GROWTH_TIME_STEPS) << " There are " << growth_time_steps_.size() << " growth time step labels but there are "
      << n_growth_episodes_ << " growth episodes. These must be the same length.";
}

/**
 * Build objects that are needed by this object during the execution phase. This
 * includes things like the partition accessor it will need.
 *
 * Then build values to be printed when print is called.
 *
 */
void GrowthBasic::DoBuild() {
  partition_.Init(category_labels_);

  // Populate length mid points
  // need to check Categories don't have difference length bins otherwise this won't work.
  length_bins_ = model_->length_bins();
  for (unsigned l = 0; l < length_bins_.size(); ++l) { // iterate over each length bin
    length_bin_mid_points_[l] = (length_bins_[l] + length_bins_[l + 1]) * 0.5;
  }

  // Build Transition Matrix so call reset
  DoReset();
}

/**
 * Reset all objects in the length growth class
 */
void GrowthBasic::DoReset() {
  // Build Transition Matrix
  Double mu, sigma;
  for (unsigned length_bin = 0; length_bin < length_bins_.size(); ++length_bin) {
    Double sum_so_far = 0.0;
    for (unsigned j = 1; j < length_bin; ++j){
      transition_matrix_[length_bin][j] = 0; // no negative growth
    }
    if (model_->length_plus() && (length_bin == (length_bins_.size() - 1))) {
      transition_matrix_[length_bin][length_bin] = 1.0; // plus group can't grow any more
    }

    // Calculate incremental change based on mid point
    mu = g_[0] + (g_[1] - g_[0])*(length_bin_mid_points_[length_bin] - l_[0]) / (l_[1] - l_[0]);
    sigma = cv_ * mu > min_sigma_ ? cv_ * mu : min_sigma_;
    transition_matrix_[length_bin][length_bin] = math::pnorm(length_bins_[length_bin + 1] - length_bin_mid_points_[length_bin],mu,sigma); // Calculate normal PDF I am using Boost not sure if this will play ball.
    sum_so_far = transition_matrix_[length_bin][length_bin];

    for (unsigned j = length_bin + 1; j < (length_bins_.size() - 1); ++j){
      transition_matrix_[length_bin][j] = math::pnorm(length_bins_[j + 1] - length_bin_mid_points_[length_bin],mu,sigma) - sum_so_far;
      sum_so_far += transition_matrix_[length_bin][j];
    }

    if (model_->length_plus()){
      transition_matrix_[length_bin][(length_bins_.size() - 1)] = 1 - sum_so_far;
    } else {
      transition_matrix_[length_bin][(length_bins_.size() - 1)] = math::pnorm(length_bins_[length_bins_.size() - 1] - length_bin_mid_points_[length_bin], mu, sigma) - sum_so_far;
    }
  }
}

/**
 * Execute the length growth class.
 */
void GrowthBasic::DoExecute() {
  // iterate over each category multiplying numbers at length by the transition matrix
  // will have to initialise numbers_transitioning_matrix_;

  // TODO: This will have to be revisited, I don't think it is very efficient.
  for (auto category : partition_) {
    unsigned this_length_bin = 0;
    for (Double& data : category->data_) {
      for (unsigned length_bin = 0; length_bin < length_bins_.size(); ++length_bin, ++this_length_bin) {
        numbers_transitioning_matrix_[this_length_bin][length_bin] = data * transition_matrix_[this_length_bin][length_bin];
      }
    }

    // Make the move
    this_length_bin = 0;
    for (Double& data : category->data_) {
      for (unsigned length_bin = 0; length_bin < length_bins_.size(); ++length_bin, ++this_length_bin) {
        data += numbers_transitioning_matrix_[length_bin][this_length_bin];
      }
    }
  }
}

} /* namespace length */
} /* namespace processes */
} /* namespace niwa */
