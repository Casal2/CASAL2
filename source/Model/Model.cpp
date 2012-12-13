/**
 * @file Model.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Model.h"

#include <iostream>

#include "Utilities/Logging/Logging.h"

// Namespaces
namespace isam {

using std::cout;
using std::endl;


Model::Model() {
  LOG_TRACE();
  parameters_.RegisterAllowed(PARAM_START_YEAR);
  parameters_.RegisterAllowed(PARAM_RUN_LENGTH);
  parameters_.RegisterAllowed(PARAM_MIN_AGE);
  parameters_.RegisterAllowed(PARAM_MAX_AGE);
  parameters_.RegisterAllowed(PARAM_AGE_PLUS);
  parameters_.RegisterAllowed(PARAM_INITIALIZATION_PHASES);
  parameters_.RegisterAllowed(PARAM_TIME_STEPS);
}

/**
 * Our singleton accessor method
 *
 * @return singleton shared ptr
 */
shared_ptr<Model> Model::Instance() {
  static ModelPtr model = ModelPtr(new Model());
  return model;
}

/**
 * Start our model. This is the entry point method for the model
 * after being called from the main() method.
 *
 * This method will start stepping through the states and verifying
 * each step.
 */
void Model::Start() {
  if (state_ != State::kInitialise)
    LOG_CODE_ERROR("Model state should always be initialise when entering the start method");

  Initialise();

  state_ = State::kValidate;
  Validate();

  state_ = State::kBuild;
  Build();

  state_ = State::kVerify;
  Verify();

  switch(run_mode_) {
  case RunMode::kBasic:
    RunBasic();
    break;

  case RunMode::kEstimation:
    RunEstimation();
    break;

  default:
    LOG_ERROR("Invalid run mode has been specified. This run mode is not supported: " << run_mode_);
    break;
  }

}

/**
 *
 */
void Model::Initialise() { }

/**
 *
 */
void Model::Validate() { }

/**
 *
 */
void Model::Build() { }

/**
 *
 */
void Model::Verify() { }

/**
 *
 */
void Model::RunBasic() {
  cout << "Running model in basic mode" << endl;

}

/**
 *
 */
void Model::RunEstimation() {
  cout << "Running model in estimation mode" << endl;
}



} /* namespace isam */
