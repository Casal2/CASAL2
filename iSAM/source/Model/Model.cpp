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
#include "Asserts/Manager.h"
#include "Catchabilities/Manager.h"
#include "Categories/Categories.h"
#include "GlobalConfiguration/GlobalConfiguration.h"

#include "DerivedQuantities/Manager.h"
#include "Estimates/Manager.h"
#include "InitialisationPhases/Manager.h"
#include "MCMC/MCMC.h"
#include "Minimisers/Manager.h"
#include "Observations/Manager.h"
#include "Penalties/Manager.h"
#include "Processes/Manager.h"
#include "Profiles/Manager.h"
#include "Projects/Manager.h"
#include "Reports/Manager.h"
#include "Selectivities/Manager.h"
#include "Simulates/Manager.h"
#include "SizeWeights/Manager.h"
#include "TimeSteps/Manager.h"
#include "Model/Executor.h"
#include "ObjectiveFunction/ObjectiveFunction.h"

#include "Partition/Accessors/Category.h"
#include "Partition/Partition.h"

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
  parameters_.Bind<unsigned>(PARAM_START_YEAR, &start_year_, "The first year of the model", "");
  parameters_.Bind<unsigned>(PARAM_FINAL_YEAR, &final_year_, "The last year of the model", "");
  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "The default minimum age for the population", "");
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "The default maximum age for the population", "");
  parameters_.Bind<bool>(PARAM_AGE_PLUS, &age_plus_, "True if the model supports an age-plus group", "", false);
  parameters_.Bind<string>(PARAM_INITIALISATION_PHASES, &initialisation_phases_, "List of initialisation phases to execute", "", true);
  parameters_.Bind<string>(PARAM_TIME_STEPS, &time_steps_, "List of time steps to execute", "");
  parameters_.Bind<unsigned>(PARAM_PROJECTION_FINAL_YEAR, &projection_final_year_, "The final year of the model in projection mode", "", 0);
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
 *
 */
vector<unsigned> Model::years() const {
  vector<unsigned> years;
  unsigned year;
  for (year = current_year_; year <= final_year_; ++year)
    years.push_back(year);
  for (; year <= projection_final_year_; ++year)
    years.push_back(year);

  return years;
}

/**
 *
 */
unsigned Model::year_spread() const {
  unsigned spread = (final_year_ - start_year_) + 1;
  if (run_mode_ == RunMode::kProjection)
    spread = (projection_final_year_ - start_year_) + 1;

  return spread;
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

  LOG_INFO("Model: State Change to Validate");
  state_ = State::kValidate;
  Validate();
  reports::Manager::Instance().Execute(state_);

  LOG_INFO("Model: State Change to Build");
  state_ = State::kBuild;
  Build();
  reports::Manager::Instance().Execute(state_);

  LOG_INFO("Model: State Change to Verify");
  state_ = State::kVerify;
  Verify();
  reports::Manager::Instance().Execute(state_);

  // prepare all reports
  LOG_INFO("Preparing Reports");
  reports::Manager::Instance().Prepare();

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

  case RunMode::kProfiling:
    RunProfiling();
    break;

  case RunMode::kSimulation:
    RunSimulation();
    break;

  case RunMode::kProjection:
    RunProjection();
    break;

  case RunMode::kTesting:
    break;

  default:
    LOG_ERROR("Invalid run mode has been specified. This run mode is not supported: " << run_mode_);
    break;
  }

  // finalise all reports
  LOG_INFO("Finalising Reports");
  state_ = State::kFinalise;
  reports::Manager::Instance().Execute(state_);
  reports::Manager::Instance().Finalise();
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

  if (run_mode_ == RunMode::kProjection) {
    if (projection_final_year_ <= start_year_ || projection_final_year_ <= final_year_) {
      LOG_ERROR(parameters_.location(PARAM_PROJECTION_FINAL_YEAR) << "("
          << projection_final_year_ << ") cannot be less than or equal to start_year (" << start_year_
          << ") or final_year (" << final_year_ << ")");
    }
  }

  // Call validation for the other objects required by the model
  Categories::Instance()->Validate();
  Partition::Instance().Validate();

  agesizes::Manager::Instance().Validate();
  asserts::Manager::Instance().Validate();
  catchabilities::Manager::Instance().Validate();
  derivedquantities::Manager::Instance().Validate();
  initialisationphases::Manager::Instance().Validate();
  minimisers::Manager::Instance().Validate();
  observations::Manager::Instance().Validate();
  penalties::Manager::Instance().Validate();
  processes::Manager::Instance().Validate();
  profiles::Manager::Instance().Validate();
  projects::Manager::Instance().Validate();
  reports::Manager::Instance().Validate();
  selectivities::Manager::Instance().Validate();
  simulates::Manager::Instance().Validate();
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
      LOG_ERROR(parameters_.location(PARAM_INITIALISATION_PHASES) << "(" << phase << ") has not been defined. Please ensure you have defined it");
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
  asserts::Manager::Instance().Build();
  catchabilities::Manager::Instance().Build();
  derivedquantities::Manager::Instance().Build();
  initialisationphases::Manager::Instance().Build();
  minimisers::Manager::Instance().Build();
  observations::Manager::Instance().Build();
  penalties::Manager::Instance().Build();
  processes::Manager::Instance().Build();
  profiles::Manager::Instance().Build();
  projects::Manager::Instance().Build();
  reports::Manager::Instance().Build();
  selectivities::Manager::Instance().Build();
  simulates::Manager::Instance().Build();
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
  asserts::Manager::Instance().Reset();
  Categories::Instance()->Reset();
  catchabilities::Manager::Instance().Reset();
  derivedquantities::Manager::Instance().Reset();
  initialisationphases::Manager::Instance().Reset();
  minimisers::Manager::Instance().Reset();
  observations::Manager::Instance().Reset();
  penalties::Manager::Instance().Reset();
  processes::Manager::Instance().Reset();
  profiles::Manager::Instance().Reset();
  projects::Manager::Instance().Reset();
  reports::Manager::Instance().Reset();
  selectivities::Manager::Instance().Reset();
  simulates::Manager::Instance().Reset();
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

  // Model has finished so we can run finalise.
  LOG_INFO("Model: State change to PostExecute");
  reports::Manager::Instance().Execute(State::kPostExecute);
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
}

/**
 *
 */
void Model::RunProfiling() {
  LOG_INFO("Doing pre-profile iteration of the model");
  Iterate();

  LOG_INFO("Entering the Profiling Sub-System");
  estimates::Manager& estimate_manager = estimates::Manager::Instance();
  MinimiserPtr minimiser = minimisers::Manager::Instance().active_minimiser();

  vector<ProfilePtr> profiles = profiles::Manager::Instance().GetObjects();
  LOG_INFO("Working with " << profiles.size() << " profiles");
  for (ProfilePtr profile : profiles) {
    LOG_INFO("Disabling estiSmate: " << profile->parameter());
    estimate_manager.DisableEstimate(profile->parameter());

    LOG_INFO("First-Stepping profile");
    profile->FirstStep();
    for (unsigned i = 0; i < profile->steps() + 2; ++i) {
      LOG_INFO("Calling minimiser to begin the estimation (profling)");
      minimiser->Execute();

      LOG_INFO("Model: State change to Iteration Complete")
      reports::Manager::Instance().Execute(State::kIterationComplete); // Change to KIterationComplete

      profile->NextStep();
    }
    profile->RestoreOriginalValue();

    estimate_manager.EnableEstimate(profile->parameter());
  }
}

/**
 *
 */
void Model::RunSimulation() {
  LOG_INFO("Entering the Simulation Sub-System");

  unsigned simulation_candidates = GlobalConfiguration::Instance()->simulation_candidates();
  unsigned suffix_width          = (unsigned)floor(log10((double) simulation_candidates + 1)) + 1;
  for (unsigned i = 0; i < simulation_candidates; ++i) {
    string report_suffix = ".";
    unsigned iteration_width = (unsigned)floor(log10(i + 1)) + 1;
    report_suffix.append("0", suffix_width - iteration_width);
    report_suffix.append(utilities::ToInline<unsigned, string>(i + 1));
    reports::Manager::Instance().set_report_suffix(report_suffix);

    Reset();

    // Model is about to run
    LOG_INFO("Model: State change to PreExecute");
    reports::Manager::Instance().Execute(State::kPreExecute);

    state_ = State::kInitialise;
    current_year_ = start_year_;
    initialisationphases::Manager& init_phase_manager = initialisationphases::Manager::Instance();
    init_phase_manager.Execute();

    state_ = State::kExecute;
    timesteps::Manager& time_step_manager = timesteps::Manager::Instance();
    for (current_year_ = start_year_; current_year_ <= final_year_; ++current_year_, current_year_index_ = current_year_ - start_year_) {
      LOG_INFO("Iteration year: " << current_year_);
      time_step_manager.Execute(current_year_);
      simulates::Manager::Instance().Update(current_year_);
    }

    observations::Manager::Instance().CalculateScores();

    // Model has finished so we can run finalise.
    LOG_INFO("Model: State change to PostExecute");
    reports::Manager::Instance().Execute(State::kPostExecute);
    reports::Manager::Instance().Execute(State::kIterationComplete);
  }
}

/**
 *
 */
void Model::RunProjection() {
  LOG_INFO("Entering the Projection Sub-System");

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

  LOG_INFO("Starting projection years")
  projects::Manager& project_manager = projects::Manager::Instance();
  for (; current_year_ <= projection_final_year_; ++current_year_) {
    LOG_INFO("Iteration year: " << current_year_);
    project_manager.Update(current_year_);
    time_step_manager.Execute(current_year_);
  }

  // Model has finished so we can run finalise.
  LOG_INFO("Model: State change to PostExecute");
  reports::Manager::Instance().Execute(State::kPostExecute);

  observations::Manager::Instance().CalculateScores();
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
  for (current_year_ = start_year_; current_year_ <= final_year_; ++current_year_, current_year_index_ = current_year_ - start_year_) {
    LOG_INFO("Iteration year: " << current_year_);
    time_step_manager.Execute(current_year_);
  }

  observations::Manager::Instance().CalculateScores();
}

/**
 *
 */
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
