/**
 * @file Binomial.cpp
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
#include <Likelihoods/Common/Binomial.h>
#include "Utilities/Math.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/RandomNumberGenerator.h"

// Namespaces
namespace niwa {
namespace likelihoods {

namespace math = niwa::utilities::math;
namespace dc = niwa::utilities::doublecompare;

/**
 * Adjust the error value based on the process error
 *
 * @param process_error The observations process_error
 * @param error_value The observations error_value
 * @return An adjusted error value
 */
Double Binomial::AdjustErrorValue(const Double process_error, const Double error_value) {
  if (error_value > 0.0 && process_error > 0.0)
    return (1.0 /  (1.0 / error_value + 1.0 / process_error));

  return error_value;
}

/**
 * Calculate the scores
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void Binomial::GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) {
  for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
    for (observations::Comparison& comparison : year_iterator->second) {
      Double error_value = AdjustErrorValue(comparison.process_error_, comparison.error_value_) * error_value_multiplier_;
      if (error_value == 0.0) {
        comparison.adjusted_error_ = error_value;
        comparison.score_ = 0.0;
      } else {
        Double score = math::LnFactorial(error_value)
                        - math::LnFactorial(error_value * (1.0 - comparison.observed_))
                        - math::LnFactorial(error_value * comparison.observed_)
                        + error_value * comparison.observed_ * log(dc::ZeroFun(comparison.expected_, comparison.delta_))
                        + error_value * (1.0 - comparison.observed_) * log(dc::ZeroFun(1.0 - comparison.expected_, comparison.delta_));

        comparison.adjusted_error_ = error_value;
        comparison.score_ = -score * multiplier_;
      }
    }
  }
}

/**
 * Simulate observed values
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void Binomial::SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  double error_value = 0.0;
  auto iterator = comparisons.begin();
  for (; iterator != comparisons.end(); ++iterator) {
    LOG_FINE() << "Simulating values for year: " << iterator->first;
    for (observations::Comparison& comparison : iterator->second) {
      error_value = ceil(AS_VALUE(AdjustErrorValue(comparison.process_error_, comparison.error_value_)));

      if (comparison.expected_ <= 0.0 || error_value <= 0.0)
        comparison.observed_ = 0.0;
      else
        comparison.observed_ = rng.binomial(AS_VALUE(comparison.expected_), error_value) / error_value;

      comparison.adjusted_error_ = error_value;
    }
  }
}

} /* namespace likelihoods */
} /* namespace niwa */
