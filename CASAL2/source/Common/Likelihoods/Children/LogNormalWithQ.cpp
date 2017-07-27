/**
 * @file LogNormalWithQ.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 25/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "LogNormalWithQ.h"

#include <cmath>

#include "Common/Utilities/DoubleCompare.h"
#include "Common/Utilities/RandomNumberGenerator.h"

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
Double LogNormalWithQ::AdjustErrorValue(const Double process_error, const Double error_value) {
  if (process_error > 0.0)
    return sqrt(error_value * error_value + process_error * process_error);

  return error_value;
}


/**
 * Get the result from our likelihood
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void LogNormalWithQ::GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) {
  for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
    for (observations::Comparison& comparison : year_iterator->second) {

      Double error_value = AdjustErrorValue(comparison.process_error_, comparison.error_value_) * error_value_multiplier_;
      Double sigma = sqrt(log(1 + error_value * error_value));
      Double score = log(comparison.observed_ / dc::ZeroFun(comparison.expected_, comparison.delta_)) / sigma + 0.5 * sigma;
      score = log(sigma) + 0.5 * (score * score);

      comparison.adjusted_error_ = error_value;
      comparison.score_ = score * multiplier_;
    }
  }
}

/**
 * Simulate observed values
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void LogNormalWithQ::SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  Double error_value = 0.0;
  auto iterator = comparisons.begin();
  for (; iterator != comparisons.end(); ++iterator) {
    LOG_FINE() << "Simulating values for year: " << iterator->first;
    for (observations::Comparison& comparison : iterator->second) {
      error_value = AdjustErrorValue(comparison.process_error_, comparison.error_value_);

      if (comparison.expected_ <= 0.0 || error_value <= 0.0)
        comparison.observed_ = comparison.delta_;
      else
        comparison.observed_ = rng.lognormal(AS_DOUBLE(comparison.expected_), AS_DOUBLE(error_value));

      comparison.adjusted_error_ = error_value;
    }
  }
}

} /* namespace likelihoods */
} /* namespace niwa */
