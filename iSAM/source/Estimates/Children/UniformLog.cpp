/**
 * @file UniformLog.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "UniformLog.h"

// Namespaces
namespace isam {
namespace estimates {

/**
 * Default Constructor
 */
UniformLog::UniformLog() {
}

/**
 * Calculate and return the score
 *
 * @param The parameter to return log of
 * @return Score as log(param)
 */
Double UniformLog::GetScore(Double param) {
  score_ = log(param);
  return score_;
}

} /* namespace estimates */
} /* namespace isam */
