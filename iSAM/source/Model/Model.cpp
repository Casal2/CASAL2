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

#include "AgeSizes/Manager.h"
#include "Catchabilities/Manager.h"
#include "Categories/Categories.h"
#include "DerivedQuantities/Manager.h"
#include "Estimates/Manager.h"
#include "InitialisationPhases/Manager.h"
#include "MCMC/MCMC.h"
#include "Minimisers/Manager.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Observations/Manager.h"
#include "Partition/Accessors/Category.h"
#include "Partition/Partition.h"
#include "Penalties/Manager.h"
#include "Processes/Manager.h"
#include "Reports/Manager.h"
#include "Selectivities/Manager.h"
#include "SizeWeights/Manager.h"
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
  parameters_.Bind<unsigned>(PARAM_START_YEAR, &start_year_, "The first year of the model");
  parameters_.Bind<unsigned>(PARAM_FINAL_YEAR, &final_year_, "The last year of the model");
  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "The default minimum age for the population");
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "The default maximum age for the population");
  parameters_.Bind<bool>(PARAM_AGE_PLUS, &age_plus_, "True if the model supports an age-plus group", false);
  parameters_.Bind<string>(PARAM_INITIALIZATION_PHASES, &initialisation_phases_, "List of initialisation phases to execute", true);
  parameters_.Bind<string>(PARAM_TIME_STEPS, &time_steps_, "List of time steps to execute");
  parameters_.Bind<unsigned>(PARAM_PROJEECTION_FINAL_YEAR, &projection_final_year_, "The final year of the model in projection mode", 0);
}

/**
 * Our singleton accessor method
 *
 * @param force_new Force a new instance or not
 * @return singleton shared ptr
 */
shared_ptr<Model> Model::Instance(bool force_new) {
  static ModelPtr model = ModelPtr(new Model());
  if (force_new)
    model.reset(new Model());

  return model;
}

/**
 * Start our model. This is the entry point method for the model
 * after being called from the main() method.
 *
 * This method will start stepping through the states and verifying
 * each step.
 */
void Model::Start(RunMode::Type run_mode) {
  LOG_TRACE();
  run_mode_ = run_mode;

  if (state_ != State::kStartUp)
    LOG_CODE_ERROR("Model state should always be startup when entering the start method");
  reports::Manager::Instance().Execute(State::kStartUp);

  state_ = State::kValidate;
  LOG_INFO("Model: State Change to Validate");
  Validate();
  reports::Manager::Instance().Execute(State::kValidate);

  state_ = State::kBuild;
  LOG_INFO("Model: State Change to Build");
  Build();
  reports::Manager::Instance().Execute(State::kBuild);

  state_ = State::kVerify;
  LOG_INFO("Model: State Change to Verify");
  Verify();
  reports::Manager::Instance().Execute(State::kVerify);

  switch(run_mode_) {
  case RunMode::kBasic:
    RunBasic();
    break;

  case RunMode::kEstimation:
    RunEstimation();
    break;

  case RunMode::kMCMC:
    RunMCMC();
    break;

  case RunMode::kTesting:
    break;

  default:
    LOG_ERROR("Invalid run mode has been specified. This run mode is not supported: " << run_mode_);
    break;
  }

}

/**
 * First we will do the local validations. Then we will call validation on the other objects
 */
void Model::Validate() {
  LOG_TRACE();

  // Check that we've actually defined a @model block
  if (block_type_ == "")
    LOG_ERROR("The @model block is missing from configuration file. This block is required.");

  parameters_.Populate();

  if (start_year_ > final_year_)
    LOG_ERROR(parameters_.location(PARAM_FINAL_YEAR) << ": final_year is before the start_year, final_year must be greater than the start_year");
  if (min_age_ > max_age_)
    LOG_ERROR(parameters_.location(PARAM_MIN_AGE) << " (" << min_age_ << ") has been defined as greater than max_age (" << max_age_ << ")");


  // Call validation for the other objects required by the model
  Categories::Instance()->Validate();
  Partition::Instance().Validate();

  agesizes::Manager::Instance().Validate();
  catchabilities::Manager::Instance().Validate();
  derivedquantities::Manager::Instance().Validate();
  initialisationphases::Manager::Instance().Validate();
  minimisers::Manager::Instance().Validate();
  observations::Manager::Instance().Validate();
  penalties::Manager::Instance().Validate();
  processes::Manager::Instance().Validate();
  reports::Manager::Instance().Validate();
  selectivities::Manager::Instance().Validate();
  sizeweights::Manager::Instance().Validate();
  timesteps::Manager::Instance().Validate();

  // Final Objects to validate as they have dependencies
  estimates::Manager::Instance().Validate();

  /**
   * Do some more sanity checks
   */
  initialisationphases::Manager& init_phase_mngr = initialisationphases::Manager::Instance();
  for (const string& phase : initialisation_phases_) {
    if (!init_phase_mngr.IsPhaseDefined(phase))
      LOG_ERROR(parameters_.location(PARAM_INITIALIZATION_PHASES) << "(" << phase << ") has not been defined. Please ensure you have defined it");
  }

  timesteps::Manager& time_step_mngr = timesteps::Manager::Instance();
  for (const string time_step : time_steps_) {
    if (!time_step_mngr.GetTimeStep(time_step))
      LOG_ERROR(parameters_.location(PARAM_TIME_STEPS) << "(" << time_step << ") has not been defined. Please ensure you have defined it");
  }
}

/**
 *
 */
void Model::Build() {
  LOG_TRACE();
  Categories::Instance()->Build();
  Partition::Instance().Build();
  timesteps::Manager::Instance().Build();

  // build managers
  estimates::Manager::Instance().Build();
  agesizes::Manager::Instance().Build();
  catchabilities::Manager::Instance().Build();
  derivedquantities::Manager::Instance().Build();
  initialisationphases::Manager::Instance().Build();
  minimisers::Manager::Instance().Build();
  observations::Manager::Instance().Build();
  penalties::Manager::Instance().Build();
  processes::Manager::Instance().Build();
  reports::Manager::Instance().Build();
  selectivities::Manager::Instance().Build();
  sizeweights::Manager::Instance().Build();

  Partition::Instance().CalculateMeanWeights();
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
void Model::Reset() {
  LOG_TRACE();

  Partition::Instance().Reset();

  estimates::Manager::Instance().Reset();

  agesizes::Manager::Instance().Reset();
  Categories::Instance()->Reset();
  catchabilities::Manager::Instance().Reset();
  derivedquantities::Manager::Instance().Reset();
  initialisationphases::Manager::Instance().Reset();
  minimisers::Manager::Instance().Reset();
  observations::Manager::Instance().Reset();
  penalties::Manager::Instance().Reset();
  processes::Manager::Instance().Reset();
  reports::Manager::Instance().Reset();
  selectivities::Manager::Instance().Reset();
  sizeweights::Manager::Instance().Reset();
  timesteps::Manager::Instance().Reset();
}

/**
 *
 */
void Model::RunBasic() {
  LOG_TRACE();
  // Model is about to run
  LOG_INFO("Model: State change to PreExecute");
  reports::Manager::Instance().Execute(State::kPreExecute);

  /**
   * Running the model now
   */
  LOG_INFO("Model: State change to Execute");
  Iterate();
  ObjectiveFunction::Instance().CalculateScore();


  processes::Manager::Instance().Print();

  // Model has finished so we can run finalise.
  LOG_INFO("Model: State change to PostExecute");
  reports::Manager::Instance().Execute(State::kPostExecute);

  LOG_INFO("Model: State change to Finalise")
  reports::Manager::Instance().Execute(State::kFinalise);
}

/**
 * Run the model in estimation mode.
 */
void Model::RunEstimation() {
  /*
   * Before running the model in estimation mode we'll do an iteration
   * as a basic model. We don't call any reports though.
   */
  LOG_INFO("Doing pre-estimation iteration of the model");
  Iterate();

  LOG_INFO("Calling minimiser to begin the estimation");
  MinimiserPtr minimiser = minimisers::Manager::Instance().active_minimiser();
  minimiser->Execute();

  LOG_INFO("Model: State change to Finalise")
  reports::Manager::Instance().Execute(State::kFinalise);

  run_mode_ = RunMode::kBasic;
  FullIteration();
}

/**
 *
 */
void Model::RunMCMC() {
  LOG_INFO("Entering the MCMC Sub-System");
  MCMCPtr mcmc = MCMC::Instance();
  mcmc->Validate();
  mcmc->Build();

  LOG_INFO("Calling minimiser to find our minimum and covariance matrix");
  MinimiserPtr minimiser = minimisers::Manager::Instance().active_minimiser();
  minimiser->Execute();
  minimiser->BuildCovarianceMatrix();

  LOG_INFO("Minimisation complete. Starting MCMC");
  mcmc->Execute();

  LOG_INFO("Model: State change to Finalise")
  reports::Manager::Instance().Execute(State::kFinalise);
}


/**
 * This method will do a single iteration of the model. During
 * a basic run it'll only run once, but during the other times
 * it'll run multiple times.
 */
void Model::Iterate() {
  LOG_TRACE();

  state_ = State::kInitialise;
  current_year_ = start_year_;
  initialisationphases::Manager& init_phase_manager = initialisationphases::Manager::Instance();
  init_phase_manager.Execute();

  state_ = State::kExecute;
  timesteps::Manager& time_step_manager = timesteps::Manager::Instance();
  for (current_year_ = start_year_; current_year_ <= final_year_; ++current_year_) {
    LOG_INFO("Iteration year: " << current_year_);
    time_step_manager.Execute(current_year_);
  }
}

void Model::FullIteration() {
  Reset();

  // Model is about to run
  LOG_INFO("Model: State change to PreExecute");
  reports::Manager::Instance().Execute(State::kPreExecute);

  /**
   * Running the model now
   */
  LOG_INFO("Model: State change to Execute");
  Iterate();

  // Model has finished so we can run finalise.
  LOG_INFO("Model: State change to PostExecute");
  reports::Manager::Instance().Execute(State::kPostExecute);
}



} /* namespace isam */
