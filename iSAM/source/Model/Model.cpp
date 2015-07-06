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

#include "AdditionalPriors/Manager.h"
#include "AgeLengths/Manager.h"
#include "AgeLengthKeys/Manager.h"
#include "Asserts/Manager.h"
#include "Catchabilities/Manager.h"
#include "Categories/Categories.h"
#include "ConfigurationLoader/EstimableValuesLoader.h"
#include "DerivedQuantities/Manager.h"
#include "Estimables/Estimables.h"
#include "Estimates/Manager.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "InitialisationPhases/Manager.h"
#include "MCMCs/Manager.h"
#include "Minimisers/Manager.h"
#include "Observations/Manager.h"
#include "Penalties/Manager.h"
#include "Processes/Manager.h"
#include "Profiles/Manager.h"
#include "Projects/Manager.h"
#include "Reports/Manager.h"
#include "Selectivities/Manager.h"
#include "Simulates/Manager.h"
#include "LengthWeights/Manager.h"
#include "TimeSteps/Manager.h"
#include "TimeVarying/Manager.h"
#include "ObjectiveFunction/ObjectiveFunction.h"

#include "Partition/Accessors/Category.h"
#include "Partition/Partition.h"

#include "Logging/Logging.h"
#include "Utilities/To.h"


// TODO: Remove
#include "Estimates/Transformations/Factory.h"


// Namespaces
namespace niwa {

using std::cout;
using std::endl;

/**
 * Default Constructor
 */
Model::Model() {
  LOG_TRACE();
  parameters_.Bind<unsigned>(PARAM_START_YEAR, &start_year_, "Define the first year of the model, immediately following initialisation", R"(Defines the first year of the model, $\ge 1$, e.g. 1990)");
  parameters_.Bind<unsigned>(PARAM_FINAL_YEAR, &final_year_, "Define the final year of the model, excluding years in the projection period", "Defines the last year of the model, i.e., the model is run from start_year to final_year");
  parameters_.Bind<unsigned>(PARAM_MIN_AGE, &min_age_, "Minimum age of individuals in the population", R"($0 \le$ age\textlow{min} $\le$ age\textlow{max})", 0);
  parameters_.Bind<unsigned>(PARAM_MAX_AGE, &max_age_, "Maximum age of individuals in the population", R"($0 \le$ age\textlow{min} $\le$ age\textlow{max})", 0);
  parameters_.Bind<bool>(PARAM_AGE_PLUS, &age_plus_, "Define the oldest age as a plus group", "true, false", false);
  parameters_.Bind<string>(PARAM_INITIALISATION_PHASES, &initialisation_phases_, "Define the labels of the phases of the initialisation", R"(A list of valid labels defined by \texttt{@initialisation_phase})", true);
  parameters_.Bind<string>(PARAM_TIME_STEPS, &time_steps_, "Define the labels of the time steps, in the order that they are applied, to form the annual cycle", R"(A list of valid labels defined by \texttt{@time_step})");
  parameters_.Bind<unsigned>(PARAM_PROJECTION_FINAL_YEAR, &projection_final_year_, "Define the final year of the model in projection mode", R"(Defines the last year of the projection period, i.e., the projection period runs from \texttt{final_year}$+1$ to \texttt{projection_final_year}. For the default, $0$, no projections are run.)", 0);
  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type of model (the partition structure). Either age, length or hybrid", "", PARAM_AGE);
  parameters_.Bind<Double>(PARAM_LENGTH_BINS, &length_bins_, "", "", true);
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
  for (year = start_year_; year <= final_year_; ++year)
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
bool Model::Start(RunMode::Type run_mode) {
  LOG_TRACE();
  Logging& logging = Logging::Instance();
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return false;
  }

  run_mode_ = run_mode;

  if (state_ != State::kStartUp)
    LOG_CODE_ERROR() << "Model state should always be startup when entering the start method";
  GlobalConfigurationPtr global_config = GlobalConfiguration::Instance();
  if (global_config->estimable_value_file() != "") {
    configuration::EstimableValuesLoader loader;
    loader.LoadValues(global_config->estimable_value_file());
  }
  reports::Manager::Instance().Execute(State::kStartUp);

  LOG_FINE() << "Model: State Change to Validate";
  state_ = State::kValidate;
  Validate();
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return false;
  }

  reports::Manager::Instance().Execute(state_);

  LOG_FINE() << "Model: State Change to Build";
  state_ = State::kBuild;
  Build();
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return false;
  }
  reports::Manager::Instance().Execute(state_);

  LOG_FINE() << "Model: State Change to Verify";
  state_ = State::kVerify;
  Verify();
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return false;
  }
  reports::Manager::Instance().Execute(state_);

  // prepare all reports
  LOG_FINE() << "Preparing Reports";
  reports::Manager::Instance().Prepare();

  switch(run_mode_) {
  case RunMode::kBasic:
    RunBasic();
    break;

  case RunMode::kEstimation:
    RunEstimation();
    break;

  case RunMode::kMCMC:
    if (!RunMCMC())
      return false;
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
    LOG_ERROR() << "Invalid run mode has been specified. This run mode is not supported: " << run_mode_;
    break;
  }

  // finalise all reports
  LOG_FINE() << "Finalising Reports";
  state_ = State::kFinalise;
  for (ExecutorPtr executor : executors_[state_])
    executor->Execute();
  reports::Manager::Instance().Execute(state_);
  reports::Manager::Instance().Finalise();
  return true;
}

/**
 * First we will do the local validations. Then we will call validation on the other objects
 */
void Model::Validate() {
  LOG_TRACE();

  // Check that we've actually defined a @model block
  if (block_type_ == "")
    LOG_ERROR() << "The @model block is missing from configuration file. This block is required.";

  /**
   * Validate the parameters
   */
  parameters_.Populate();
  if (type_ == PARAM_AGE)
    partition_structure_ = PartitionStructure::kAge;
  else if (type_ == PARAM_LENGTH)
    partition_structure_ = PartitionStructure::kLength;
  else
    LOG_ERROR_P(PARAM_TYPE) << " (" << type_ << ") is not valid. Please use either " << PARAM_AGE
        << " or " << PARAM_LENGTH;

  if (partition_structure_ == PartitionStructure::kAge) {
    if (start_year_ < 1000)
      LOG_ERROR_P(PARAM_START_YEAR) << " (" << start_year_ << ") cannot be less than 1000";
    if (start_year_ > final_year_)
      LOG_ERROR_P(PARAM_FINAL_YEAR) << " (" << final_year_ << ") cannot be less than start_year (" << start_year_ << ")";
    if (min_age_ > max_age_)
      LOG_ERROR_P(PARAM_MIN_AGE) << " (" << min_age_ << ") has been defined as greater than max_age (" << max_age_ << ")";

    if (run_mode_ == RunMode::kProjection) {
      if (projection_final_year_ <= start_year_ || projection_final_year_ <= final_year_) {
        LOG_ERROR_P(PARAM_PROJECTION_FINAL_YEAR) << "("
            << projection_final_year_ << ") cannot be less than or equal to start_year (" << start_year_
            << ") or final_year (" << final_year_ << ")";
      }
    }

  } else if (partition_structure_ == PartitionStructure::kLength) {
    if (!parameters_.Get(PARAM_LENGTH_BINS)->has_been_defined())
      LOG_ERROR() << location() << "@model is missing required parameter " << PARAM_LENGTH_BINS;
    if (parameters_.Get(PARAM_MIN_AGE)->has_been_defined())
      LOG_ERROR_P(PARAM_MIN_AGE) << "cannot be defined in a length model";
    if (parameters_.Get(PARAM_MAX_AGE)->has_been_defined())
      LOG_ERROR_P(PARAM_MAX_AGE) << "cannot be defined in a length model";
    if (parameters_.Get(PARAM_AGE_PLUS)->has_been_defined())
      LOG_ERROR_P(PARAM_AGE_PLUS) << "cannot be defined in a length model";

  } else
    LOG_ERROR() << "Partition structure " << (unsigned)partition_structure_ << " not supported";

  // Call validation for the other objects required by the model
  Categories::Instance()->Validate();
  Partition::Instance().Validate();
  timesteps::Manager::Instance().Validate();

  additionalpriors::Manager::Instance().Validate();
  agelengths::Manager::Instance().Validate();
  agelengthkeys::Manager::Instance().Validate();
  asserts::Manager::Instance().Validate();
  catchabilities::Manager::Instance().Validate();
  derivedquantities::Manager::Instance().Validate();
  initialisationphases::Manager::Instance().Validate();
  lengthweights::Manager::Instance().Validate();
  mcmcs::Manager::Instance().Validate();
  minimisers::Manager::Instance().Validate();
  observations::Manager::Instance().Validate();
  penalties::Manager::Instance().Validate();
  processes::Manager::Instance().Validate();
  profiles::Manager::Instance().Validate();
  projects::Manager::Instance().Validate();
  reports::Manager::Instance().Validate();
  selectivities::Manager::Instance().Validate();
  simulates::Manager::Instance().Validate();
  timevarying::Manager::Instance().Validate();

  // Final Objects to validate as they have dependencies
  estimates::Manager::Instance().Validate();

  /**
   * Do some more sanity checks
   */
  initialisationphases::Manager& init_phase_mngr = initialisationphases::Manager::Instance();
  for (const string& phase : initialisation_phases_) {
    if (!init_phase_mngr.IsPhaseDefined(phase))
      LOG_ERROR_P(PARAM_INITIALISATION_PHASES) << "(" << phase << ") has not been defined. Please ensure you have defined it";
  }

  timesteps::Manager& time_step_mngr = timesteps::Manager::Instance();
  for (const string time_step : time_steps_) {
    if (!time_step_mngr.GetTimeStep(time_step))
      LOG_ERROR_P(PARAM_TIME_STEPS) << "(" << time_step << ") has not been defined. Please ensure you have defined it";
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
  additionalpriors::Manager::Instance().Build();
  estimates::Manager::Instance().Build();
  agelengths::Manager::Instance().Build();
  agelengthkeys::Manager::Instance().Build();
  asserts::Manager::Instance().Build();
  catchabilities::Manager::Instance().Build();
  derivedquantities::Manager::Instance().Build();
  initialisationphases::Manager::Instance().Build();
  lengthweights::Manager::Instance().Build();
  mcmcs::Manager::Instance().Build();
  minimisers::Manager::Instance().Build();
  observations::Manager::Instance().Build();
  penalties::Manager::Instance().Build();
  processes::Manager::Instance().Build();
  profiles::Manager::Instance().Build();
  projects::Manager::Instance().Build();
  reports::Manager::Instance().Build();
  selectivities::Manager::Instance().Build();
  simulates::Manager::Instance().Build();
  timevarying::Manager::Instance().Build();

  EstimablesPtr estimables = Estimables::Instance();
  if (estimables->GetValueCount() > 0) {
    estimable_values_file_ = true;
    estimable_values_count_ = estimables->GetValueCount();
  }
}

/**
 *
 */
void Model::Verify() {
  LOG_TRACE();
  for (ExecutorPtr executor : executors_[state_])
    executor->Execute();
}

/**
 *
 */
void Model::Reset() {
  LOG_TRACE();

  Partition::Instance().Reset();

  estimates::Manager::Instance().Reset();

  additionalpriors::Manager::Instance().Reset();
  agelengths::Manager::Instance().Reset();
  agelengthkeys::Manager::Instance().Reset();
  asserts::Manager::Instance().Reset();
  Categories::Instance()->Reset();
  catchabilities::Manager::Instance().Reset();
  derivedquantities::Manager::Instance().Reset();
  initialisationphases::Manager::Instance().Reset();
  lengthweights::Manager::Instance().Build();
  mcmcs::Manager::Instance().Reset();
  minimisers::Manager::Instance().Reset();
  observations::Manager::Instance().Reset();
  penalties::Manager::Instance().Reset();
  processes::Manager::Instance().Reset();
  profiles::Manager::Instance().Reset();
  projects::Manager::Instance().Reset();
  reports::Manager::Instance().Reset();
  selectivities::Manager::Instance().Reset();
  simulates::Manager::Instance().Reset();
  timesteps::Manager::Instance().Reset();
  timevarying::Manager::Instance().Reset();
}

/**
 *
 */
void Model::RunBasic() {
  LOG_TRACE();
  EstimablesPtr estimables = Estimables::Instance();

  // Model is about to run
  for (unsigned i = 0; i < estimable_values_count_; ++i) {
    if (estimable_values_file_) {
      estimables->LoadValues(i);
      Reset();
    }

    LOG_FINE() << "Model: State change to PreExecute";
    reports::Manager::Instance().Execute(State::kPreExecute);

    /**
     * Running the model now
     */
    LOG_FINE() << "Model: State change to Execute";
    Iterate();
    ObjectiveFunction::Instance().CalculateScore();

    // Model has finished so we can run finalise.
    LOG_FINE() << "Model: State change to PostExecute";
    reports::Manager::Instance().Execute(State::kPostExecute);

    LOG_FINE() << "Model: State change to Iteration Complete";
    reports::Manager::Instance().Execute(State::kIterationComplete);
  }
}

/**
 * Run the model in estimation mode.
 */
void Model::RunEstimation() {
  GlobalConfiguration::Instance()->set_force_estimable_values_file();

  /*
   * Before running the model in estimation mode we'll do an iteration
   * as a basic model. We don't call any reports though.
   */
  LOG_FINE() << "Doing pre-estimation iteration of the model";
  Iterate();

  EstimablesPtr estimables = Estimables::Instance();
  map<string, Double> estimable_values;
  for (unsigned i = 0; i < estimable_values_count_; ++i) {
    if (estimable_values_file_) {
      estimables->LoadValues(i);
      Reset();
    }

    LOG_FINE() << "Calling minimiser to begin the estimation with the " << i + 1 << "st/nd/nth set of values";
    run_mode_ = RunMode::kEstimation;

    MinimiserPtr minimiser = minimisers::Manager::Instance().active_minimiser();
    minimiser->Execute();

    run_mode_ = RunMode::kBasic;
    FullIteration();

    LOG_FINE() << "Model: State change to Iteration Complete";
    reports::Manager::Instance().Execute(State::kIterationComplete);
  }
}

/**
 *
 */
bool Model::RunMCMC() {
  LOG_FINE() << "Entering the MCMC Sub-System";
  MCMCPtr mcmc = mcmcs::Manager::Instance().active_mcmc();

  Logging& logging = Logging::Instance();
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return false;
  }

  LOG_FINE() << "Calling minimiser to find our minimum and covariance matrix";
  MinimiserPtr minimiser = minimisers::Manager::Instance().active_minimiser();
  minimiser->Execute();
  minimiser->BuildCovarianceMatrix();

  LOG_FINE() << "Minimisation complete. Starting MCMC";
  mcmc->Execute();
  return true;
}

/**
 *
 */
void Model::RunProfiling() {
  GlobalConfiguration::Instance()->set_force_estimable_values_file();

  EstimablesPtr estimables = Estimables::Instance();

  map<string, Double> estimable_values;
  for (unsigned i = 0; i < estimable_values_count_; ++i) {
    if (estimable_values_file_) {
      estimables->LoadValues(i);
      Reset();
    }

    LOG_FINE() << "Doing pre-profile iteration of the model";
    Iterate();

    LOG_FINE() << "Entering the Profiling Sub-System";
    estimates::Manager& estimate_manager = estimates::Manager::Instance();
    MinimiserPtr minimiser = minimisers::Manager::Instance().active_minimiser();

    vector<ProfilePtr> profiles = profiles::Manager::Instance().objects();
    LOG_FINE() << "Working with " << profiles.size() << " profiles";
    for (ProfilePtr profile : profiles) {
      LOG_FINE() << "Disabling estiSmate: " << profile->parameter();
      estimate_manager.DisableEstimate(profile->parameter());

      LOG_FINE() << "First-Stepping profile";
      profile->FirstStep();
      for (unsigned i = 0; i < profile->steps() + 2; ++i) {
        LOG_FINE() << "Calling minimiser to begin the estimation (profling)";
        minimiser->Execute();

        run_mode_ = RunMode::kBasic;
        FullIteration();

        LOG_FINE() << "Model: State change to Iteration Complete";
        reports::Manager::Instance().Execute(State::kIterationComplete);

        run_mode_ = RunMode::kProfiling;
        profile->NextStep();
      }
      profile->RestoreOriginalValue();

      estimate_manager.EnableEstimate(profile->parameter());
    }
  }
}

/**
 *
 */
void Model::RunSimulation() {
  LOG_FINE() << "Entering the Simulation Sub-System";

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
    LOG_FINE() << "Model: State change to PreExecute";
    reports::Manager::Instance().Execute(State::kPreExecute);

    state_ = State::kInitialise;
    current_year_ = start_year_;
    initialisationphases::Manager& init_phase_manager = initialisationphases::Manager::Instance();
    init_phase_manager.Execute();

    state_ = State::kExecute;
    timesteps::Manager& time_step_manager = timesteps::Manager::Instance();
    timevarying::Manager& time_varying_manager = timevarying::Manager::Instance();
    for (current_year_ = start_year_; current_year_ <= final_year_; ++current_year_, current_year_index_ = current_year_ - start_year_) {
      LOG_FINE() << "Iteration year: " << current_year_;
      time_varying_manager.Update(current_year_);
      simulates::Manager::Instance().Update(current_year_);
      time_step_manager.Execute(current_year_);
    }

    observations::Manager::Instance().CalculateScores();

    // Model has finished so we can run finalise.
    LOG_FINE() << "Model: State change to PostExecute";
    reports::Manager::Instance().Execute(State::kPostExecute);
    reports::Manager::Instance().Execute(State::kIterationComplete);
  }
}

/**
 *
 */
void Model::RunProjection() {
  LOG_FINE() << "Entering the Projection Sub-System";

  state_ = State::kInitialise;
  current_year_ = start_year_;
  initialisationphases::Manager& init_phase_manager = initialisationphases::Manager::Instance();
  init_phase_manager.Execute();

  state_ = State::kExecute;
  timesteps::Manager& time_step_manager = timesteps::Manager::Instance();
  timevarying::Manager& time_varying_manager = timevarying::Manager::Instance();
  for (current_year_ = start_year_; current_year_ <= final_year_; ++current_year_) {
    LOG_FINE() << "Iteration year: " << current_year_;
    time_varying_manager.Update(current_year_);
    time_step_manager.Execute(current_year_);
  }

  LOG_FINE() << "Starting projection years";
  projects::Manager& project_manager = projects::Manager::Instance();
  for (; current_year_ <= projection_final_year_; ++current_year_) {
    LOG_FINE() << "Iteration year: " << current_year_;
    time_varying_manager.Update(current_year_);
    project_manager.Update(current_year_);
    time_step_manager.Execute(current_year_);
  }

  // Model has finished so we can run finalise.
  LOG_FINE() << "Model: State change to PostExecute";
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
  timevarying::Manager& time_varying_manager = timevarying::Manager::Instance();
  for (current_year_ = start_year_; current_year_ <= final_year_; ++current_year_, current_year_index_ = current_year_ - start_year_) {
    LOG_FINE() << "Iteration year: " << current_year_;
    time_varying_manager.Update(current_year_);
    time_step_manager.Execute(current_year_);
  }

  observations::Manager::Instance().CalculateScores();

  for (ExecutorPtr executor : executors_[State::kExecute])
    executor->Execute();
}

/**
 *
 */
void Model::FullIteration() {
  Reset();

  // Model is about to run
  LOG_FINE() << "Model: State change to PreExecute";
  reports::Manager::Instance().Execute(State::kPreExecute);

  /**
   * Running the model now
   */
  LOG_FINE() << "Model: State change to Execute";
  Iterate();

  // Model has finished so we can run finalise.
  LOG_FINE() << "Model: State change to PostExecute";
  reports::Manager::Instance().Execute(State::kPostExecute);
}
} /* namespace niwa */
