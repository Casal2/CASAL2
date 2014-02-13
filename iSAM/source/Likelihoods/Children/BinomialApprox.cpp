/**
 * @file BinomialApprox.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "BinomialApprox.h"

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
Double BinomialApprox::AdjustErrorValue(const Double process_error, const Double error_value) {
  if (error_value > 0.0 && process_error > 0.0)
    return (1.0 / (1.0 / error_value + 1.0 / process_error));

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
void BinomialApprox::GetResult(vector<Double> &scores, const vector<Double> &expecteds, const vector<Double> &observeds,
                              const vector<Double> &error_values, const vector<Double> &process_errors, const Double delta) {

  Double error_value = 0.0;
  Double std_error   = 0.0;
  Double score       = 0.0;

  for (unsigned i = 0; i < expecteds.size(); ++i) {
    error_value = AdjustErrorValue(process_errors[i], error_values[i]);
    std_error   = sqrt((dc::ZeroFun(expecteds[i], delta) * dc::ZeroFun(1.0 - expecteds[i], delta)) / error_value);
    score       = log(std_error) + 0.5 * pow((observeds[i] - expecteds[i]) / std_error, 2.0);

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
void BinomialApprox::SimulateObserved(const vector<string> &keys, vector<Double> &observeds, const vector<Double> &expecteds,
                              const vector<Double> &error_values, const vector<Double> &process_errors, const Double delta) {

  utilities::RandomNumberGenerator& rnd = utilities::RandomNumberGenerator::Instance();
  Double error_value = 0;
  Double observed    = 0;

  observeds.clear();
  for (unsigned i = 0; i < expecteds.size(); ++i) {
   error_value = AdjustErrorValue(process_errors[i], error_values[i]);
   if (error_values[i] < 0.0)
     LOG_ERROR(this->location() << ": error_value (" << AS_DOUBLE(error_values[i]) << ") was less than 0.0 when simulating an observed value (i = " << i << ")");

   if (expecteds[i] <= 0.0 || error_value <= 0.0) {
     observeds.push_back(0.0);
     continue;
   }

   observed = rnd.binomial(AS_DOUBLE(expecteds[i]), AS_DOUBLE(error_value));
   observeds.push_back(observed);
  }
}

/**
 * Simulate observed values
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void BinomialApprox::SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();

  Double error_value = 0.0;
  auto iterator = comparisons.begin();
  for (; iterator != comparisons.end(); ++iterator) {
    LOG_INFO("Simulating values for year: " << iterator->first);
    for (observations::Comparison& comparison : iterator->second) {
      error_value = AdjustErrorValue(comparison.process_error_, comparison.error_value_);

      if (comparison.expected_ <= 0.0 || error_value <= 0.0)
        comparison.observed_ = 0.0;
      else
        comparison.observed_ = rng.binomial(AS_DOUBLE(comparison.expected_), AS_DOUBLE(error_value));
    }
  }
}

} /* namespace likelihoods */
} /* namespace isam */
