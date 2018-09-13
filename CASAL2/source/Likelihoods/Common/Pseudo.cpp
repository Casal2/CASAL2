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
#include <Likelihoods/Common/Pseudo.h>

// Namespaces
namespace niwa {
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
 * Simulate observed values
 *
 * @param comparisons A collection of comparisons passed by the observation
 */
void Pseudo::SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) {
  auto iterator = comparisons.begin();
  for (; iterator != comparisons.end(); ++iterator) {
    LOG_FINE() << "Simulating values for year: " << iterator->first;
    for (observations::Comparison& comparison : iterator->second) {
      comparison.observed_ = 0.0;
    }
  }
}


} /* namespace likelihoods */
} /* namespace niwa */
