/**
 * @file LogNormal.cpp
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
#include "LogNormal.h"

#include <cmath>

#include "Utilities/DoubleCompare.h"
#include "Utilities/RandomNumberGenerator.h"

// Namespaces
namespace isam {
namespace likelihoods {

namespace dc = isam::utilities::doublecompare;

/**
 * Adjust the error value based on the process error
 *
 * @param process_error The observations process_error
 * @param error_value The observations error_value
 * @return An adjusted error value
 */
Double LogNormal::AdjustErrorValue(const Double process_error, const Double error_value) {
  if (process_error > 0.0)
    return sqrt(error_value * error_value + process_error * process_error);

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
void LogNormal::GetResult(vector<Double> &scores, const vector<Double> &expecteds, const vector<Double> &observeds,
                              const vector<Double> &error_values, const vector<Double> &process_errors, const Double delta) {

  Double error_value  = 0.0;
  Double sigma        = 0.0;
  Double score        = 0.0;

  for (unsigned i = 0; i < expecteds.size(); ++i) {
    error_value = AdjustErrorValue(process_errors[i], error_values[i]);
    sigma       = sqrt(log(1 + error_value * error_value));
    score       = log(observeds[i] / dc::ZeroFun(expecteds[i], delta)) / sigma + 0.5 * sigma;
    score       = log(sigma) + 0.5 * score * score;

    scores.push_back(score);
  }
}

/**
 * Get the result from our likelihood
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void LogNormal::GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) {
  for (auto year_iterator = comparisons.begin(); year_iterator != comparisons.end(); ++year_iterator) {
    for (observations::Comparison& comparison : year_iterator->second) {

      Double error_value = AdjustErrorValue(comparison.process_error_, comparison.error_value_);
      Double sigma = sqrt(log(1 + error_value * error_value));
      Double score = log(comparison.observed_ / dc::ZeroFun(comparison.expected_, comparison.delta_)) / sigma + 0.5 * sigma;
      score = log(sigma) + 0.5 * (score * score);
      comparison.score_ = score;
    }
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
void LogNormal::SimulateObserved(const vector<string> &keys, vector<Double> &observeds, const vector<Double> &expecteds,
                              const vector<Double> &error_values, const vector<Double> &process_errors, const Double delta) {

  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  observeds.clear();

  Double error_value = 0.0;
  Double observed    = 0.0;
  for (unsigned i = 0; i < expecteds.size(); ++i) {
    error_value = AdjustErrorValue(process_errors[i], error_values[i]);

    if (expecteds[i] <= 0.0 || error_value < 0.0) {
      observeds.push_back(0.0);
      continue;
    }

    observed = rng.binomial(AS_DOUBLE(expecteds[i]), AS_DOUBLE(error_value));
    observeds.push_back(observed);
  }
}

/**
 * Simulate observed values
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void LogNormal::SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  auto iterator = comparisons.begin();
  for (; iterator != comparisons.end(); ++iterator) {
    LOG_INFO("Simulating values for year: " << iterator->first);

    map<string, Double> totals;
    for (observations::Comparison& comparison : iterator->second) {
      Double error_value = AdjustErrorValue(comparison.process_error_, comparison.error_value_);

      if (comparison.expected_ <= 0.0 || error_value <= 0.0)
        comparison.observed_ = comparison.delta_;
      else
        comparison.observed_ = rng.lognormal(AS_DOUBLE(comparison.expected_), AS_DOUBLE(error_value));

      totals[comparison.category_] += comparison.observed_;
    }

    for (observations::Comparison& comparison : iterator->second)
      comparison.observed_ /= totals[comparison.category_];
  }
}

} /* namespace likelihoods */
} /* namespace isam */
