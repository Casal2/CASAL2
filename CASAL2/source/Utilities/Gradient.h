/**
 * @file Gradient.h
 * @author Scott Rasmussen
 * @brief Contains the different gradient calculation functions
 * for the different model types. Because Casal2 supports multiple
 * types of models, we need multiple ways to calculate gradient
 * functions in a multi-threaded environment
 * @version 0.1
 * @date 2021-04-24
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef UTILITIES_GRADIENT_H_
#define UTILITIES_GRADIENT_H_

// headers
#include <memory>
#include <vector>

// Forward Declare model
namespace niwa {
class Model;
class ThreadPool;
}  // namespace niwa

// namespace
namespace niwa::utilities::gradient {

// function declarations
std::vector<double> Calculate(std::shared_ptr<ThreadPool> thread_pool, std::vector<double> estimate_values, std::vector<double> lower_bounds, std::vector<double> upper_bounds,
                              double step_size, double last_score, bool values_are_scaled = false, bool truncate_values_when_scaling = false);

// Utility Functions
double UnScaleValue(const double& value, double min, double max);

}  // namespace niwa::utilities::gradient
#endif /* UTILITIES_GRADIENT_H_ */
