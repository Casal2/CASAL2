/**
 * @file Poisson.cpp
 * @author  C Marsh
 * @version 1.0
 * @date 2023
 * @section LICENSE
 *
 * Copyright NIWA Science ©2023 - www.niwa.co.nz
 *
 */

// Headers
#include "Poisson.h"

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
Double Poisson::AdjustErrorValue(const Double process_error, const Double error_value) {
  return 0.0;
}

/**
 * Simulate observed values
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void Poisson::SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  auto iterator = comparisons.begin();
  for (; iterator != comparisons.end(); ++iterator) {
    LOG_FINE() << "Simulating values for year: " << iterator->first;
    for (observations::Comparison& comparison : iterator->second) {
      comparison.observed_ = rng.poisson(AS_DOUBLE(comparison.expected_));
    }
  }
}

/**
 * Calculate the scores
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void Poisson::GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) {
  // Year iterator
  for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
    // obs block iterator
    for (observations::Comparison& comparison : year_iterator->second) {
      // check expected isn't zero
      comparison.expected_ = math::ZeroFun(comparison.expected_, comparison.delta_);
      //comparison.adjusted_error_ = comparison.expected_;// lambda is the variance
      comparison.score_ = -1.0 * (-1.0 * comparison.expected_ + comparison.observed_ * log(comparison.expected_) - math::LnGamma(comparison.observed_ + 1.0));
      //std::cerr << " observed = " << comparison.observed_ << " expected " <<  comparison.expected_ << " score " << comparison.score_ << "\n";
      comparison.score_ *= multiplier_;
    }
  }
}
} /* namespace likelihoods */
} /* namespace niwa */
