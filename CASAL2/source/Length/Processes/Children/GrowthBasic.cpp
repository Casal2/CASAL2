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

#include "Common/Utilities/To.h"
#include "Common/Model/Managers.h"
#include "Common/Categories/Categories.h"
#include "Common/TimeSteps/Manager.h"
#include "Common/Utilities/Map.h"
// #include "boost/math/distributions/normal.hpp"
#include "Common/Utilities/Math.h"

// Namespaces
namespace niwa {
namespace length {
namespace processes {

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
  parameters_.Bind<unsigned>(PARAM_NUMBER_OF_GROWTH_EPISODES, &number_of_growth_episodes_, "Number of growth episodes per year", "");
  parameters_.Bind<string>(PARAM_GROWTH_TIME_STEPS, &growth_time_steps_, "Time step in which each growth episode occurs", "");
  parameters_.Bind<Double>(PARAM_CV, &cv_ , "c.v. for the growth model", "",Double(0.0))->set_lower_bound(0.0);
  parameters_.Bind<Double>(PARAM_SIGMA_MIN, &min_sigma_ , "Lower bound on sigma for the growth model", "",Double(0.0));
  parameters_.Bind<Double>(PARAM_L_REF, &l_ref_ , "Two reference lengths", R"(l_\textlow{ref}[1] $\le$ l_\textlow{ref}[2])");
  parameters_.Bind<Double>(PARAM_G_REF, &g_ref_ , "Growth rates at reference lengths", R"(g_\textlow{ref}[1] $\le$ g_\textlow{ref}[2])");
}

/**
 * Validate our Growth Process process.
 *
 * 1. Check for any required parameters
 * 2. Assign the label from our parameters
 * 3. Check categories
 */
void GrowthBasic::DoValidate() {
  if (growth_time_steps_.size() != number_of_growth_episodes_)
      LOG_ERROR_P(PARAM_GROWTH_TIME_STEPS) << "You supplied " << growth_time_steps_.size() << " time step labels but only have " << number_of_growth_episodes_ << " in the model. These need to be the same";
  if (l_ref_.size() != 2)
      LOG_ERROR_P(PARAM_L_REF) << "You supplied " << l_ref_.size() << " reference lengths (l_ref) but there should be 2.";
  if (g_ref_.size() != 2)
      LOG_ERROR_P(PARAM_G_REF) << "You supplied " << g_ref_.size() << " reference growth rates (g_ref) but there should be 2.";
}

/**
 * Build objects that are needed by this object during the execution phase. This
 * includes things like the partition accessor it will need.
 *
 * Then build values that we want to print when print is called.
 *
 */
void GrowthBasic::DoBuild() {
  LOG_FINEST() << "GrowthBasic DoBuild started";
  partition_.Init(category_labels_);
  LOG_FINEST() << "TEST 1";

  // Populate length mid points
  // need to check Categories don't have difference length bins otherwise this won't work.
  length_bins_ = model_->length_bins(); // check that inputting length bin vector works
  LOG_FINEST() << "Number of length bins: " << length_bins_.size();
  for (unsigned l = 0; l < length_bins_.size() - 1; ++l) { // iterate over each length bin
    length_bin_mid_points_[l] = (length_bins_[l] + length_bins_[l + 1]) * 0.5;
    LOG_FINEST() << "Length bin mid point " << l <<": " << length_bin_mid_points_[l];
  }
  if (model_->length_plus()) { // presence of plus group
    length_bin_mid_points_[length_bins_.size() - 1] = model_->length_plus_group();
    LOG_FINEST() << "Plus group present, length bin " << length_bins_.size() - 1 <<": " << length_bin_mid_points_[length_bins_.size() - 1];
  }
  LOG_FINEST() << "TEST 3";
  // Build Transition Matrix so call reset
  DoReset();
  LOG_FINEST() << "GrowthBasic DoBuild finished";
}

void GrowthBasic::DoReset() {

// /*
  LOG_FINEST() << "GrowthBasic DoReset started";
  // Build Transition Matrix
  Double mu, sigma;
  transition_matrix_[length_bins_.size(), length_bins_.size()]; // make matrix right size?
  LOG_FINEST() << "Size of transition matrix is " << transition_matrix_.size();
  for (unsigned length_bin = 0; length_bin < length_bins_.size(); ++length_bin) {
    LOG_FINEST() << "length_bin = " << length_bin;
    Double sum_so_far = 0.0;
    for (unsigned j = 1; j < length_bin; ++j){
      transition_matrix_[length_bin][j] = 0; // no negative growth
      LOG_FINEST() << "transition_matrix_[" << length_bin << "][" << j << "] = " << transition_matrix_[length_bin][j];
    }
    if (model_->length_plus() && (length_bin == (length_bins_.size() - 1))) {
      transition_matrix_[length_bin][length_bin] = 1.0; // plus group can't grow any more
      LOG_FINEST() << "transition_matrix_[" << length_bin << "][" << length_bin << "] = " << transition_matrix_[length_bin][length_bin];
    }
    // Calculate incremental change based on mid point
    mu = g_ref_[0] + (g_ref_[1] - g_ref_[0])*(length_bin_mid_points_[length_bin] - l_ref_[0]) / (l_ref_[1] - l_ref_[0]);
    LOG_FINEST() << "mu = " << mu;
    sigma = cv_ * mu;
    if (sigma < min_sigma_) // lower limit of min_sigma_
      sigma = min_sigma_;
    LOG_FINEST() << "sigma = " << sigma;
    // Build boost object.
    // boost::math::normal norm{mu, sigma};
    transition_matrix_[length_bin][length_bin] = 0.012; // Calculate normal CDF using pnorm function
    LOG_FINEST() << "transition_matrix_[" << length_bin << "][" << length_bin << "] = " << transition_matrix_[length_bin][length_bin];
    transition_matrix_[length_bin][length_bin] = math::pnorm(length_bins_[length_bin + 1] - length_bin_mid_points_[length_bin], mu, sigma); // Calculate normal CDF using pnorm function
    LOG_FINEST() << "transition_matrix_[" << length_bin << "][" << length_bin << "] = " << transition_matrix_[length_bin][length_bin]; // Calculate normal CDF using pnorm function
    LOG_FINEST() << "TEST 5";
    sum_so_far = transition_matrix_[length_bin][length_bin];

    for (unsigned j = length_bin + 1; j < (length_bins_.size() - 1); ++j){
      transition_matrix_[length_bin][j] = math::pnorm(length_bins_[j + 1] - length_bin_mid_points_[length_bin], mu, sigma) - sum_so_far;
      sum_so_far += transition_matrix_[length_bin][j];
    }
    if (model_->length_plus()){
      transition_matrix_[length_bin][(length_bins_.size() - 1)] = 1 - sum_so_far;
    } else {
      transition_matrix_[length_bin][(length_bins_.size() - 1)] = math::pnorm(length_bins_[length_bins_.size() - 1] - length_bin_mid_points_[length_bin], mu, sigma) - sum_so_far;
    }
  }
// */
  LOG_FINEST() << "TEST 3";
  for (unsigned j = 1; j < length_bins_.size(); ++j) {
    for (unsigned k = 1; k < length_bins_.size(); ++k) {
      LOG_FINEST() << "Transition matrix, row " << j << ", column " << k << ", value " << transition_matrix_[j][k];
    }
  }
  LOG_FINEST() << "Check model start year: " << model_->start_year();
  LOG_FINEST() << "GrowthBasic DoReset finished";
}

/**
 * Execute our ageing class.
 */
void GrowthBasic::DoExecute() {
  // iterate over each category multiplying numbers at length by the transition matrix
  // will have to initialise numbers_transitioning_matrix_;

  // TODO: This will have to be revisited, I don't think it is very efficient.
  LOG_FINEST() << "GrowthBasic DoExecute started";
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
  LOG_FINEST() << "GrowthBasic DoExecute finished";
}

} /* namespace processes */
} /* namespace length */
} /* namespace niwa */
