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

#include "Categories/Categories.h"
#include "InitialisationPhases/Manager.h"
#include "Partition/Accessors/Category.h"
#include "Partition/Partition.h"
#include "Processes/Manager.h"
#include "Selectivities/Manager.h"
#include "TimeSteps/Manager.h"
#include "Utilities/Logging/Logging.h"
#include "Utilities/To.h"

// Namespaces
namespace isam {

using std::cout;
using std::endl;

/**
 * Default Constructor
 */
Model::Model() {
  LOG_TRACE();
  parameters_.RegisterAllowed(PARAM_START_YEAR);
  parameters_.RegisterAllowed(PARAM_FINAL_YEAR);
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
  LOG_TRACE();

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
 * This method will initialise everything that is needed to be done before the Validation starts.
 */
void Model::Initialise() {
  LOG_TRACE();
}

/**
 * First we will do the local validations. Then we will call validation on the other objects
 */
void Model::Validate() {
  LOG_TRACE();

  // Check that we've actually defined a @model block
  if (block_type_ == "")
    LOG_ERROR("The @model block is missing from configuration file. This block is required.");

  // Validate our own parameters
  CheckForRequiredParameter(PARAM_START_YEAR);
  CheckForRequiredParameter(PARAM_FINAL_YEAR);
  CheckForRequiredParameter(PARAM_MIN_AGE);
  CheckForRequiredParameter(PARAM_MAX_AGE);

  // Validate: start_year
  start_year_ = parameters_.Get(PARAM_START_YEAR).GetValue<unsigned>();
  final_year_ = parameters_.Get(PARAM_FINAL_YEAR).GetValue<unsigned>();
  min_age_    = parameters_.Get(PARAM_MIN_AGE).GetValue<unsigned>();
  max_age_    = parameters_.Get(PARAM_MAX_AGE).GetValue<unsigned>();

  if (start_year_ > final_year_) {
    LOG_ERROR(parameters_.location(PARAM_FINAL_YEAR) << ": final_year is before the start_year, final_year must be greater than the start_year");
  }

  if (min_age_ > max_age_) {
    Parameter min_age = parameters_.Get(PARAM_MIN_AGE);
    Parameter max_age = parameters_.Get(PARAM_MAX_AGE);

    LOG_ERROR("At line " << max_age.line_number() << " in file " << max_age.file_name()
        << ": max_age is less than the min_age defined at line " << min_age.line_number() << " in file " << min_age.file_name());
  }

  // Call validation for the other objects required by the model
  Categories::Instance()->Validate();
  Partition::Instance().Validate();

  initialisationphases::Manager::Instance().Validate();
  processes::Manager::Instance().Validate();
  selectivities::Manager::Instance().Validate();
  timesteps::Manager::Instance().Validate();
}

/**
 *
 */
void Model::Build() {
  LOG_TRACE();

  Partition::Instance().Build();

  initialisationphases::Manager::Instance().Build();
  processes::Manager::Instance().Build();
  selectivities::Manager::Instance().Build();
  timesteps::Manager::Instance().Build();

  isam::partition::accessors::Category category_accessor;

}

/**
 *
 */
void Model::Verify() {
  LOG_TRACE();

}

/**
 *
 */
void Model::RunBasic() {
  LOG_TRACE();
  cout << "Running model in basic mode" << endl;

}

/**
 *
 */
void Model::RunEstimation() {
  cout << "Running model in estimation mode" << endl;
}

/**
 * This method will do a single iteration of the model. During
 * a basic run it'll only run once, but during the other times
 * it'll run multiple times.
 */
void Model::Iterate() {

  initialisationphases::Manager& init_phase_manager = initialisationphases::Manager::Instance();
  for (unsigned phase = 0; init_phase_manager.count(); ++phase) {
//    init_phase_manager->ExecuteProcesses(phase);
  }

  timesteps::Manager& time_step_manager = timesteps::Manager::Instance();
  for (unsigned year = start_year_; year <= final_year_; ++year) {
//    time_step_manager->ExecuteProcesses(year);
  }


}



} /* namespace isam */
