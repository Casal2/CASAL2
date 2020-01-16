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
#include <Likelihoods/Common/Multinomial.h>
#include <cmath>
#include <set>

#include "Utilities/DoubleCompare.h"
#include "Utilities/Math.h"
#include "Utilities/RandomNumberGenerator.h"

// Namespaces
namespace niwa {
namespace likelihoods {

using std::set;
namespace dc = niwa::utilities::doublecompare;
namespace math = niwa::utilities::math;

/**
 * Adjust the error value based on the process error
 *
 * @param process_error The observations process_error
 * @param error_value The observations error_value
 * @return An adjusted error value
 */
Double Multinomial::AdjustErrorValue(const Double process_error, const double error_value) {
  if (process_error > 0.0 && error_value > 0.0)
    return (1.0/(1.0/error_value + 1.0/process_error));

  return error_value;
}

/**
 *
 */
void Multinomial::GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) {
  for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
    for (observations::Comparison& comparison : year_iterator->second) {
      Double error_value = AdjustErrorValue(comparison.process_error_, comparison.error_value_) * error_value_multiplier_;

      Double score = math::LnFactorial(error_value * comparison.observed_)
                      - error_value * comparison.observed_ * log(dc::ZeroFun(comparison.expected_, comparison.delta_));

      comparison.adjusted_error_ = error_value;
      comparison.score_ = score * multiplier_;
    }
  }
}

/**
 * Grab the initial score for this likelihood
 *
 * @param comparisons A collection of comparisons passed by the observation
 */

Double Multinomial::GetInitialScore(map<unsigned, vector<observations::Comparison> >& comparisons, unsigned year) {
  Double score = 0.0;


 // int stopper = 0;
  observations::Comparison& comparison = comparisons[year][0];
    //if (stopper == 1)
    //  break;
    Double temp_score = -math::LnFactorial(AdjustErrorValue(comparison.process_error_, comparison.error_value_)  * error_value_multiplier_);
    LOG_FINEST() << "Adding: " << temp_score << " = LnFactorial(AdjustErrorValue(" << comparison.process_error_
      << ", " << comparison.error_value_ << ")  * " << error_value_multiplier_ << ")";
    score += temp_score;
    //stopper += 1;
  //}

  return score * multiplier_;
}

/**
 * Simulate observed values
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void Multinomial::SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  auto iterator = comparisons.begin();
  for (; iterator != comparisons.end(); ++iterator) {
    LOG_FINE() << "Simulating values for year: " << iterator->first;

//    map<string, Double> totals;
    for (observations::Comparison& comparison : iterator->second) {
      Double error_value = AdjustErrorValue(comparison.process_error_, comparison.error_value_);
      comparison.adjusted_error_ = error_value;

      if (comparison.expected_ <= 0.0 || error_value <= 0.0)
        comparison.observed_ = 0.0;
      else {
        LOG_FINEST() << "Expected = " << comparison.expected_;
        comparison.observed_ = rng.binomial(AS_VALUE(comparison.expected_), AS_VALUE(error_value));
        LOG_FINEST() << "Simulated = " << comparison.observed_;

      }
//      totals[comparison.category_] += comparison.observed_;
    }

//    for (observations::Comparison& comparison : iterator->second)
//      comparison.observed_ /= totals[comparison.category_];
  }
}

} /* namespace likelihoods */
} /* namespace niwa */
