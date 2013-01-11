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

} /* namespace isam */
