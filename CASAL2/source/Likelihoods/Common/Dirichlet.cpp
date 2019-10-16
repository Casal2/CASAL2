//============================================================================
// Name        : Dirichlet.cpp
// Author      : C.Marsh
// Date        : 21/07/2015
// Copyright   : Copyright NIWA Science ©2009 - www.niwa.co.nz
// Description :
//============================================================================

// Global headers
#include <Likelihoods/Common/Dirichlet.h>
#include <cmath>

// Local headers
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
Double Dirichlet::AdjustErrorValue(const Double process_error, const Double error_value) {
  if ((error_value > 0.0) && (process_error > 0.0))
    return (error_value * process_error);

  return error_value;
}



/*
* Get the result from our likelihood for the observation
* @param comparisons A collection of comparisons passed by the observation
*/

void Dirichlet::GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) {
  for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
    for (observations::Comparison& comparison : year_iterator->second) {
      Double error_value = AdjustErrorValue(comparison.process_error_, comparison.error_value_) * error_value_multiplier_;
      Double alpha = dc::ZeroFun(comparison.expected_,comparison.delta_) * error_value;
      Double a2_a3 = math::LnGamma(alpha) - ((alpha - 1.0) * log(dc::ZeroFun(comparison.observed_,comparison.delta_)));

      comparison.adjusted_error_ = error_value;
      comparison.score_ = a2_a3 * multiplier_;
    }
  }
}


/**
 * Simulate observed values
 *
 * @param comparisons A collection of comparisons passed by the observation
 */

void Dirichlet::SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) {
  // instance the random number generator
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  map<string, Double> totals;

  auto iterator = comparisons.begin();
  for (; iterator != comparisons.end(); ++iterator) {
    LOG_FINE() << "Simulating values for year: " << iterator->first;
    for (observations::Comparison& comparison : iterator->second) {
      Double error_value = AdjustErrorValue(comparison.process_error_, comparison.error_value_);
      if (comparison.expected_ <= 0.0 || error_value <= 0.0)
        comparison.observed_ = 0.0;
      else
        comparison.observed_ = rng.gamma(AS_VALUE(comparison.expected_) * AS_VALUE(error_value));

      totals[comparison.category_] += comparison.observed_;
      comparison.adjusted_error_ = error_value;
    }

    for (observations::Comparison& comparison : iterator->second)
      comparison.observed_ /= totals[comparison.category_];
  }
}

/**
 * Grab the initial score for this likelihood
 *
 * @param comparisons A collection of comparisons passed by the observation
 */

Double Dirichlet::GetInitialScore(map<unsigned, vector<observations::Comparison> >& comparisons,unsigned year) {
  Double score = 0.0;
  Double a1 = 0.0;

  for (observations::Comparison& comparison : comparisons[year]) {
    // Calculate score
    Double temp_score = AdjustErrorValue(comparison.process_error_, comparison.error_value_) * error_value_multiplier_;
    LOG_FINEST() << "Adding: " << temp_score << " = AdjustErrorValue(" << comparison.process_error_ << ", " << comparison.error_value_ << ")  * " << error_value_multiplier_ << ")";
    a1 += dc::ZeroFun(comparison.expected_, comparison.delta_) * temp_score;
  }

  score  = -math::LnGamma(a1);
  return score * multiplier_;
}

} /* namespace likelihoods */
} /* namespace niwa */
