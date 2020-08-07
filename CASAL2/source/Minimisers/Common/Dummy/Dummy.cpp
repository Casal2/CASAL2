/**
 * @file Dummy.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Feb 9, 2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 */

// headers
#include <Minimisers/Common/Dummy/Dummy.h>

// namespaces
namespace niwa {
namespace minimisers {

/**
 * Default constructor
 *
 * @param model Pointer to the model object
 */
Dummy::Dummy(Model* model) : Minimiser(model) {
  /**
   * Setting this parameter will allow us to create
   * dummy minimisers for types not supported by the current library.
   * (e.g. create cppad on release library).
   *
   * This is done, so we can do validation on the number of minimisers
   * loaded. This allows us to figure out what shared library to load for
   * Estimation runs.
   */
  parameters_.set_ignore_all_parameters();
}

/**
 * Execute
 */
void Dummy::Execute() {
  LOG_CODE_ERROR() << "The Dummy minimiser has been executed. This is bad.\n"
      << "This may have happened because of using an autodiff minimiser from the release shared library";
};

} /* namespace minimisers */
} /* namespace niwa */
