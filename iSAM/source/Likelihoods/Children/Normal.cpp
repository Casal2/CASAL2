/**
 * @file Normal.cpp
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
#include "Normal.h"

#include "Utilities/DoubleCompare.h"
#include "Utilities/RandomNumberGenerator.h"

// Namespaces
namespace niwa {
namespace likelihoods {

namespace dc = niwa::utilities::doublecompare;

/**
 * Adjust the error value based on the process error
 *
 * @param process_error The observations process_error
 * @param error_value The observations error_value
 * @return An adjusted error value
 */
Double Normal::AdjustErrorValue(const Double process_error, const Double error_value) {
  if (process_error > 0.0)
    return sqrt(error_value * error_value + process_error * process_error);

  return error_value;
}

/**
 * Get the result from our likelihood
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void Normal::GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) {
  for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
    for (observations::Comparison& comparison : year_iterator->second) {

      Double error_value = AdjustErrorValue(comparison.process_error_, comparison.error_value_) * error_value_multiplier_;
      Double sigma = error_value * comparison.expected_;
      Double score = (comparison.observed_ - comparison.expected_) / dc::ZeroFun(error_value * comparison.expected_, comparison.delta_);
      score = log(sigma) + 0.5 * (score * score);
      comparison.score_ = score * multiplier_;
    }
  }
}

/**
 * Simulate observed values
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void Normal::SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  Double error_value = 0.0;
  auto iterator = comparisons.begin();
  for (; iterator != comparisons.end(); ++iterator) {
    LOG_FINE() << "Simulating values for year: " << iterator->first;
    for (observations::Comparison& comparison : iterator->second) {
      error_value = AdjustErrorValue(comparison.process_error_, comparison.error_value_);

      if (comparison.expected_ <= 0.0 || error_value <= 0.0)
        comparison.observed_ = 0.0;
      else
        comparison.observed_ = rng.normal(AS_DOUBLE(comparison.expected_), AS_DOUBLE((comparison.expected_ * error_value)));
    }
  }
}

} /* namespace likelihoods */
} /* namespace niwa */
