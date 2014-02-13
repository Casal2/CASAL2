/**
 * @file Pseudo.cpp
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
#include "Pseudo.h"

// Namespaces
namespace isam {
namespace likelihoods {

/**
 * Adjust the error value based on the process error
 *
 * @param process_error The observations process_error
 * @param error_value The observations error_value
 * @return An adjusted error value
 */
Double Pseudo::AdjustErrorValue(const Double process_error, const Double error_value) {
  return 0.0;
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
void Pseudo::GetResult(vector<Double> &scores, const vector<Double> &expecteds, const vector<Double> &observeds,
                              const vector<Double> &error_values, const vector<Double> &process_errors, const Double delta) {
  scores.assign(expecteds.size(), 0.0);
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
void Pseudo::SimulateObserved(const vector<string> &keys, vector<Double> &observeds, const vector<Double> &expecteds,
                              const vector<Double> &error_values, const vector<Double> &process_errors, const Double delta) {

}

/**
 * Simulate observed values
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void Pseudo::SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) {
  auto iterator = comparisons.begin();
  for (; iterator != comparisons.end(); ++iterator) {
    LOG_INFO("Simulating values for year: " << iterator->first);
    for (observations::Comparison& comparison : iterator->second) {
      comparison.observed_ = 0.0;
    }
  }
}


} /* namespace likelihoods */
} /* namespace isam */
