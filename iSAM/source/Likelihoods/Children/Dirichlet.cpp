//============================================================================
// Name        : Dirichlet.cpp
// Author      : C.Marsh
// Date        : 21/07/2015
// Copyright   : Copyright NIWA Science ©2009 - www.niwa.co.nz
// Description :
//============================================================================

// Global headers
#include <cmath>

// Local headers
#include "Dirichlet.h"
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
	  if( (error_value > 0.0) && (process_error > 0.0) )
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
	      Double Alpha = dc::ZeroFun(comparison.expected_,comparison.delta_) * error_value;
	      Double A2_A3 = math::LnGamma(Alpha) - ((Alpha - 1.0) * log(dc::ZeroFun(comparison.expected_,comparison.delta_)));
	      comparison.score_ = A2_A3 * multiplier_;
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

  auto iterator = comparisons.begin();
  for (; iterator != comparisons.end(); ++iterator) {
    LOG_FINE() << "Simulating values for year: " << iterator->first;

    for (observations::Comparison& comparison : iterator->second) {
      Double error_value = ceil(AdjustErrorValue(comparison.process_error_, comparison.error_value_));


     if (comparison.expected_ <= 0.0 || error_value <= 0.0)
        comparison.observed_ = 0.0;

    else
        comparison.observed_ = rng.gamma(AS_DOUBLE(comparison.expected_) * AS_DOUBLE(error_value));
  }
 }
}

/**
 * Grab the initial score for this likelihood
 *
 * @param comparisons A collection of comparisons passed by the observation
 */

Double Dirichlet::GetInitialScore(map<unsigned, vector<observations::Comparison> >& comparisons,unsigned year) {
  Double score = 0.0;
  Double A1 = 0.0;

  string last_category = "";
  for (observations::Comparison& comparison : comparisons[year]) {
    if (last_category == comparison.category_)
      continue;

      last_category = comparison.category_;
      // Calculate score
      Double temp_score = AdjustErrorValue(comparison.process_error_, comparison.error_value_) * error_value_multiplier_;
      LOG_FINEST() << "Adding: " << temp_score << " = AdjustErrorValue(" << comparison.process_error_ << ", " << comparison.error_value_ << ")  * " << error_value_multiplier_ << ")";
      A1 += dc::ZeroFun(comparison.expected_, comparison.delta_) * temp_score;
    }
    score  = math::LnGamma(A1);
    return score * multiplier_;
  }

} /* namespace likelihoods */
} /* namespace niwa */
