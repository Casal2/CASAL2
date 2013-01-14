/**
 * @file Selectivity.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 11/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Selectivity.h"

// Namesapces
namespace isam {

/**
 * Default Constructor
 */
Selectivity::Selectivity() {
  parameters_.RegisterAllowed(PARAM_LABEL);
}

/**
 * Destructor
 */
Selectivity::~Selectivity() {
}

/**
 * Return the cached value for the specified age or length from
 * our internal map
 *
 * @param age_or_length The age or length to get selectivity value for
 * @return The value stored in the map or 0.0 as default
 */
Double Selectivity::GetResult(unsigned age_or_length) {
  return values_[age_or_length];
}

} /* namespace isam */
