/**
 * @file Free.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 12/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Free.h"

// namespaces
namespace niwa {
namespace catchabilities {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g profiling, yields, projections etc)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate Latex for the documentation.
 */
Free::Free() {
  parameters_.Bind<Double>(PARAM_Q, &q_, "The value of the catchability", "");

  RegisterAsEstimable(PARAM_Q, &q_);
}

} /* namespace catchabilities */
} /* namespace niwa */
