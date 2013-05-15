/**
 * @file Multinomial.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 25/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Multinomial.h"

#include <cmath>

#include "Utilities/DoubleCompare.h"
#include "Utilities/Math.h"
#include "Utilities/RandomNumberGenerator.h"

// Namespaces
namespace isam {
namespace likelihoods {

namespace dc = isam::utilities::doublecompare;
namespace math = isam::utilities::math;

/**
 * Adjust the error value based on the process error
 *
 * @param process_error The observations process_error
 * @param error_value The observations error_value
 * @return An adjusted error value
 */
Double Multinomial::AdjustErrorValue(const Double process_error, const Double error_value) {
  if (process_error > 0.0)
    return sqrt(dc::ZeroFun(error_value * error_value + process_error * process_error));

  return error_value;
}

/**
 * Get the result from our likelihood
 *
 * @param scores (out) The list of scores to populate
 * @param expecteds The list of expected values (values calculated by the model)
 * @param observeds The list of observed values supplied in the configuration file
 * @param error_values error values calculated during the observation
 * @param process_errors Process error provided to the observation
 * @param delta Delta for use int he zeroFund() utilities method
 */
void Multinomial::GetResult(vector<Double> &scores, const vector<Double> &expecteds, const vector<Double> &observeds,
                              const vector<Double> &error_values, const vector<Double> &process_errors, const Double delta) {

  Double error_value  = 0.0;
  Double score        = 0.0;

  for (unsigned i = 0; i < expecteds.size(); ++i) {
    error_value = AdjustErrorValue(process_errors[i], error_values[i]);
    score       = math::LnFactorial(error_value * observeds[i])
                  - error_value * observeds[i] * log(dc::ZeroFun(expecteds[i], delta));

    scores.push_back(score);
  }
}

/**
 * Simulate some observed values based on what the model calculated
 *
 * @param keys Unused in this method (contains categories for simulating)
 * @param observeds (out) The observed values to simulate
 * @param expecteds The list of values calculated by the model
 * @param error_values Error values calculated in the observation
 * @param process_errors Process errors passed to observation from the configuration file
 * @param delta Delta for use in the zeroFun() utilities method
 */
void Multinomial::SimulateObserved(const vector<string> &keys, vector<Double> &observeds, const vector<Double> &expecteds,
                              const vector<Double> &error_values, const vector<Double> &process_errors, const Double delta) {

  unsigned start_key    = 0;
  string   last_key     = "";

  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  observeds.assign(observeds.size(), 0.0);

  Double random_number  = 0.0;
  Double n              = 0.0;
  Double cumulative_sum = 0.0;

  for (unsigned i = 0; i < expecteds.size(); ++i) {
    for (unsigned j = start_key; j < expecteds.size(); ++j) {

      if (keys[j] != last_key) {
        last_key = keys[j];
        start_key = j;
        n = ceil(AS_DOUBLE(AdjustErrorValue(process_errors[j], error_values[j])));

        for (unsigned k = 0; k < n; ++k) {
          random_number   = rng.uniform();
          cumulative_sum  = 0.0;

          for (unsigned l = 0; l < expecteds.size(); ++l) {
            if (keys[l] == last_key) {
              cumulative_sum += expecteds[l];
              if (random_number <= cumulative_sum) {
                observeds[l] += 1.0;
                break;
              }
            }
          }
        }
      }
    }
  }

  for (unsigned i = 0; i < observeds.size(); ++i)
    observeds[i] = observeds[i] / ceil(AS_DOUBLE(AdjustErrorValue(process_errors[i], error_values[i])));
}

/**
 * Grab the initial score for this likelihood
 *
 * @param keys Unused in this method (contains categories for simulating)
 * @param process_errors Process errors passed to observation from the configuration file
 * @param error_values Error values calculated in the observation
 */
Double Multinomial::GetInitialScore(const vector<string> &keys, const vector<Double> &process_errors,
                              const vector<Double> &error_values) {

  Double score      = 0.0;
  string last_key   = "";

  for (unsigned i = 0; i < keys.size(); ++i) {
    if (keys[i] == last_key)
      continue;

    score += -math::LnFactorial(AdjustErrorValue(process_errors[i], error_values[i]));
    last_key = keys[i];
  }

  return score;
}

} /* namespace likelihoods */
} /* namespace isam */
