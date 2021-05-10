/**
 * @file Gradient.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-04-24
 *
 * @copyright Copyright (c) 2021
 *
 */

// headers
#include "Gradient.h"

#include <iomanip>
#include <iostream>

#include "../Model/Model.h"
#include "../ThreadPool/ThreadPool.h"
#include "Math.h"

// macros
#define COND_ASSIGN(res, cond, arg1, arg2) res = (cond) > 0 ? arg1 : arg2

// namespaces
namespace niwa::utilities::gradient {

using std::cout;
using std::endl;

/**
 * @brief
 *
 * @param value
 * @param min
 * @param max
 * @return double
 */
double ScaleValue(double value, double min, double max) {
  if (math::IsEqual(value, min))
    return -1;
  else if (math::IsEqual(value, max))
    return 1;

  return asin(2 * (value - min) / (max - min) - 1) / 1.57079633;
}

/**
 * @brief
 *
 * @param value
 * @param min
 * @param max
 * @param penalty
 * @return double
 */
double UnScaleValue(const double& value, double min, double max, double& penalty) {
  double t = 0.0;
  double y = 0.0;

  t = min + (max - min) * (sin(value * 1.57079633) + 1) / 2;
  COND_ASSIGN(y, -.9999 - value, (value + .9999) * (value + .9999), 0);
  penalty += y;
  COND_ASSIGN(y, value - .9999, (value - .9999) * (value - .9999), 0);
  penalty += y;
  COND_ASSIGN(y, -1 - value, 1e5 * (value + 1) * (value + 1), 0);
  penalty += y;
  COND_ASSIGN(y, value - 1, 1e5 * (value - 1) * (value - 1), 0);
  penalty += y;

  return t;
}

/**
 * @brief
 *
 * @param scaled_values
 * @param lower_bounds
 * @param upper_bounds
 */
vector<double> BuildValuesForIteration(std::vector<double> scaled_values, std::vector<double> lower_bounds, std::vector<double> upper_bounds, double penalty) {
  vector<double> current_values = scaled_values;

  for (unsigned i = 0; i < scaled_values.size(); ++i) {
    if (math::IsEqual(lower_bounds[i], upper_bounds[i]))
      current_values[i] = lower_bounds[i];
    else
      current_values[i] = UnScaleValue(scaled_values[i], lower_bounds[i], upper_bounds[i], penalty);
  }

  return current_values;
}

/**
 * @brief This method will calculate the gradient for the given candidate values using the thread_pool
 * object to do it concurrently.
 *
 * @param thread_pool object to pass the candidates to in order to have the models run
 * @param estimate_values current values of the estimates. These are modified to build gradient candidates
 * @param lower_bounds for the estimates
 * @param upper_bounds for the estimates
 * @param step_size how much to move each variable by when calculating gradient
 * @param last_score The last objective function score from the model
 * @return std::vector<double> list of gradient values for each parameter
 */
std::vector<double> Calculate(std::shared_ptr<ThreadPool> thread_pool, std::vector<double> estimate_values, std::vector<double> lower_bounds, std::vector<double> upper_bounds,
                              double step_size, double last_score) {
  long double original_value;
  long double step_size_temp;

  vector<vector<double>> gradient_candidates;
  vector<double>         gradient_penalties;

  vector<double> result_gradient(estimate_values.size(), 0.0);
  vector<double> scaled_values(estimate_values.size(), 0.0);

  // Scale the estimate values between -1 and 1 so that we can move them equally
  // based on step size
  for (unsigned i = 0; i < estimate_values.size(); ++i) {
    scaled_values[i] = ScaleValue(estimate_values[i], lower_bounds[i], upper_bounds[i]);
  }
  for (unsigned i = 0; i < estimate_values.size(); ++i) {
    step_size_temp = step_size * ((scaled_values[i] > 0) ? 1 : -1);

    // Backup Orig Value, and Assign New Var
    original_value = scaled_values[i];
    scaled_values[i] += step_size_temp;
    step_size_temp = scaled_values[i] - original_value;

    // build the candidate parameters for this variable
    double         penalty    = 0.0;
    vector<double> candidates = BuildValuesForIteration(scaled_values, lower_bounds, upper_bounds, penalty);
    gradient_penalties.push_back(penalty);
    gradient_candidates.push_back(candidates);

    scaled_values[i] = original_value;
  }  // for (int i = 0; i < iVectorSize; ++i)
  vector<double> gradient_scores(scaled_values.size(), 0.0);
  thread_pool->RunCandidates(gradient_candidates, gradient_scores);

  long double score = 0;
  for (unsigned i = 0; i < scaled_values.size(); ++i) {
    step_size_temp = step_size * ((scaled_values[i] > 0) ? 1 : -1);

    // Backup Orig Value, and Assign New Var
    original_value = scaled_values[i];
    scaled_values[i] += step_size_temp;
    step_size_temp = scaled_values[i] - original_value;

    score = gradient_scores[i];
    score += gradient_penalties[i];

    // Populate Gradient, and Restore Orig Value
    result_gradient[i] = (score - last_score) / step_size_temp;
    scaled_values[i]   = original_value;
  }
  return result_gradient;
}

}  // namespace niwa::utilities::gradient