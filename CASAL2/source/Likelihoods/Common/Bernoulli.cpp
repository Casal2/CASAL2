/**
 * @file Bernoulli.cpp
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */

// Headers
#include "Bernoulli.h"

#include "../../Utilities/Math.h"
#include "../../Utilities/RandomNumberGenerator.h"

// Namespaces
namespace niwa {
namespace likelihoods {

namespace math = niwa::utilities::math;

/**
 * Adjust the error value based on the process error
 *
 * @param process_error The observations process_error
 * @param error_value The observations error_value
 * @return An adjusted error value
 */
Double Bernoulli::AdjustErrorValue(const Double process_error, const Double error_value) {
  return 1.0;
}

/**
 * Calculate the scores
 *
 * @param comparisons A collection of comparisons passed by the observation
 *
 */
void Bernoulli::GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) {
  for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
    for (observations::Comparison& comparison : year_iterator->second) {
      Double error_value = AdjustErrorValue(comparison.process_error_, comparison.error_value_);
      if(comparison.observed_ == 1.0) {
        Double score = log(math::ZeroFun(comparison.expected_, comparison.delta_));
        comparison.adjusted_error_ = error_value;
        comparison.score_          = -score;
      } else if (comparison.observed_ == 0.0) {
        Double score = log(math::ZeroFun(1.0 - comparison.expected_, comparison.delta_));
        comparison.adjusted_error_ = error_value;
        comparison.score_          = -score;
      } else {
        LOG_FATAL() << "found an observed value = " << comparison.observed_  << " that wasn't '1' or '0' which is expected for the Bernoulli likelihood. This is not allowed";
      }
    }
  }
}

/**
 * Simulate observed values
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void Bernoulli::SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  auto   iterator    = comparisons.begin();
  for (; iterator != comparisons.end(); ++iterator) {
    LOG_FINE() << "Simulating values for year: " << iterator->first;
    for (observations::Comparison& comparison : iterator->second) {
      if(rng.uniform() <= comparison.expected_) {
        comparison.observed_ = 1.0;
      } else {
        comparison.observed_ = 0.0;
      }
    }
  }
}

} /* namespace likelihoods */
} /* namespace niwa */
