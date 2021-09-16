/**
 * @file Model.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Model.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <chrono>
#include <iostream>
#include <thread>

#include "../Categories/Categories.h"
#include "../ConfigurationLoader/EstimableValuesLoader.h"
#include "../ConfigurationLoader/MCMCObjective.h"
#include "../ConfigurationLoader/MCMCSample.h"
#include "../EquationParser/EquationParser.h"
#include "../Estimables/Estimables.h"
#include "../Estimates/Manager.h"
#include "../GlobalConfiguration/GlobalConfiguration.h"
#include "../InitialisationPhases/Manager.h"
#include "../Logging/Logging.h"
#include "../MCMCs/Manager.h"
#include "../Minimisers/Manager.h"
#include "../ObjectiveFunction/ObjectiveFunction.h"
#include "../Observations/Manager.h"
#include "../Partition/Accessors/All.h"
#include "../Partition/Accessors/Category.h"
#include "../Partition/Partition.h"
#include "../Profiles/Manager.h"
#include "../AgeLengths/Manager.h"
#include "../Projects/Manager.h"
#include "../Reports/Manager.h"
#include "../Simulates/Manager.h"
#include "../TimeSteps/Manager.h"
#include "../TimeVarying/Manager.h"
#include "../Utilities/RandomNumberGenerator.h"
#include "../Utilities/To.h"
#include "Factory.h"
#include "Managers.h"
#include "Objects.h"

// Namespaces
namespace niwa {

using std::cout;
using std::endl;

/**
 * Default Constructor
 */
Model::Model() {
  LOG_TRACE();

  parameters_.Bind<string>(PARAM_TYPE, &type_, "Type of model (only type=age is currently implemented)", "", PARAM_AGE)->set_allowed_values({PARAM_AGE, PARAM_PI_APPROX});
  // was: ->set_allowed_values({PARAM_AGE, PARAM_LENGTH, PARAM_HYBRID, PARAM_MULTIVARIATE, PARAM_PI_APPROX});
  parameters_
      .Bind<string>(PARAM_BASE_UNITS, &base_weight_units_,
                    "Define the units for the base weight measurement unit (grams, kilograms (kgs), or tonnes). This will be the default unit of any weight input values ", "",
                    PARAM_TONNES)
      ->set_allowed_values({PARAM_GRAMS, PARAM_TONNES, PARAM_KILOGRAMS, PARAM_KGS});
  parameters_.Bind<unsigned>(PARAM_THREADS, &threads_, "The number of threads to use for this model", "", 1u)->set_lower_bound(1);
}

/**
 * Destructor
 */
Model::~Model() {
  delete objects_;
  delete factory_;
  delete categories_;
  delete partition_;
  delete objective_function_;
}

GlobalConfiguration& Model::global_configuration() {
  if (global_configuration_ == nullptr)
    LOG_CODE_ERROR() << "global_configuration_ == nullptr";

  return *global_configuration_;
}

/**
 * Return the years this model is going to run in a vector.
 * We return a vector because the years will be compared
 * against processes etc that may run only on some years
 *
 * When we're running in projection mode, add the projection
 * years as well.
 */
vector<unsigned> Model::years() const {
  vector<unsigned> years;
  unsigned         year;
  for (year = start_year_; year <= final_year_; ++year) years.push_back(year);
  if (run_mode_ == RunMode::kProjection) {
    for (; year <= projection_final_year_; ++year) years.push_back(year);
  }
  return years;
}

/**
 * Return the years this model is going to run in a vector.
 * We return a vector because the years will be compared
 * against processes etc that may run only on some years
 *
 * Include the projection years, regardless of the type
 * of model run. This is used to validate the configuration
 * files when objects may be specified in a projection year
 * but the run is not a projection. We'd allow this as the
 * file is still valid
 */
vector<unsigned> Model::years_all() const {
  vector<unsigned> years;
  unsigned         year;
  for (year = start_year_; year <= final_year_; ++year) years.push_back(year);
  for (; year <= projection_final_year_; ++year) years.push_back(year);

  return years;
}

/**
 * Return the spread of years, including projection years
 * if the run mode is a projection run mode.
 */
unsigned Model::year_spread() const {
  unsigned spread = (final_year_ - start_year_) + 1;
  if (run_mode_ == RunMode::kProjection)
    spread = (projection_final_year_ - start_year_) + 1;

  return spread;
}

/**
 *
 */
shared_ptr<Managers> Model::managers() {
  LOG_TRACE();
  if (!managers_) {
    LOG_FINE() << "!managers_, so creating new pointer";
    managers_.reset(new Managers(pointer()));
  }

  return managers_;
}

Objects& Model::objects() {
  LOG_TRACE();
  if (objects_ == nullptr)
    objects_ = new Objects(pointer());
  return *objects_;
}

Factory& Model::factory() {
  LOG_TRACE();
  if (factory_ == nullptr)
    factory_ = new Factory(pointer());
  return *factory_;
}

Partition& Model::partition() {
  if (partition_ == nullptr)
    partition_ = new Partition(pointer());

  return *partition_;
}

ObjectiveFunction& Model::objective_function() {
  if (objective_function_ == nullptr)
    objective_function_ = new ObjectiveFunction(pointer());

  return *objective_function_;
}

EquationParser& Model::equation_parser() {
  if (equation_parser_ == nullptr)
    equation_parser_ = new EquationParser(pointer());

  return *equation_parser_;
}

Categories* Model::categories() {
  if (categories_ == nullptr)
    categories_ = new Categories(pointer());

  return categories_;
}

/**
 * This method will prepare the model for iterations to be run.
 * This takes the model through the Startup, Validation and Build states.
 * When we've finished this method. We're ready to run a model iteration
 * in any run mode. This is used by the Thread objects to prepare
 * the models once, then allow the Minimisers/MCMCs etc to call Iterations
 * over and over
 */
bool Model::PrepareForIterations() {
  LOG_TRACE();

  Logging& logging = Logging::Instance();
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return false;
  }

  LOG_MEDIUM() << "Model::PrepareForIterations() on thread " << std::this_thread::get_id();

  // Make sure we've instantiated our pointers to sub objects
  managers();
  objects();
  categories();
  factory();
  partition();
  objective_function();
  equation_parser();

  LOG_FINEST() << "Going into startup";
  if (state_ != State::kStartUp)
    LOG_CODE_ERROR() << "Model state should always be startup when entering the start method, not " << state_;
  if (global_configuration_->get_free_parameter_input_file() != "") {
    LOG_MEDIUM() << "get_free_parameter_input_file(): " << global_configuration_->get_free_parameter_input_file();
    configuration::EstimableValuesLoader loader(pointer());
    loader.LoadValues(global_configuration_->get_free_parameter_input_file());
  }

  managers_->report()->Execute(pointer(), State::kStartUp);

  LOG_FINE() << "Model: State Change to Validate";
  state_ = State::kValidate;
  Validate();
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return false;
  }

  managers_->report()->Execute(pointer(), state_);

  LOG_FINE() << "Model: State Change to Build";
  state_ = State::kBuild;
  Build();
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return false;
  }
  managers_->report()->Execute(pointer(), state_);

  LOG_FINE() << "Model: State Change to Verify";
  state_ = State::kVerify;
  Verify();
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return false;
  }
  managers_->report()->Execute(pointer(), state_);

  // prepare all reports
  LOG_FINE() << "Preparing Reports";
  managers_->report()->Prepare(pointer());

  return true;
}

/**
 * Start our model. This is the entry point method for the model
 * after being called from the main() method.
 *
 * This method will start stepping through the states and verifying
 * each step.
 */
bool Model::Start(RunMode::Type run_mode) {
  run_mode_ = run_mode;
#ifdef TESTMODE
  // TODO: Remove this later
  /**
   * We're hacking this in for now because the unit tests do not know about
   * the runner and threading yet.
   */
  if (state_ == State::kStartUp)
    PrepareForIterations();
#endif

  LOG_TRACE();
  switch (run_mode_) {
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

  Finalise();
  return true;
}

void Model::Finalise() {
  // finalise all reports
  LOG_FINE() << "Finalising Reports";
  state_ = State::kFinalise;
  for (auto executor : executors_[state_]) executor->Execute();
  managers_->report()->Execute(pointer(), state_);
  managers_->report()->Finalise(pointer());
}
/**
 * Populate the loaded parameters
 */
void Model::PopulateParameters() {
  LOG_TRACE();

  // Check that we've actually defined a @model block
  if (block_type_ == "")
    LOG_ERROR() << "The @model block is missing from configuration file. This block is required.";

  /**
   * Validate the parameters
   */
  parameters_.Populate(pointer());
  if (partition_type_ == PartitionType::kAge) {
  } else if (partition_type_ == PartitionType::kLength) {
    if (!parameters_.Get(PARAM_LENGTH_BINS)->has_been_defined())
      LOG_ERROR() << location() << "@model is missing required parameter " << PARAM_LENGTH_BINS;
    if (parameters_.Get(PARAM_MIN_AGE)->has_been_defined())
      LOG_ERROR_P(PARAM_MIN_AGE) << "cannot be defined in a length model";
    if (parameters_.Get(PARAM_MAX_AGE)->has_been_defined())
      LOG_ERROR_P(PARAM_MAX_AGE) << "cannot be defined in a length model";

  }  // else
  //	LOG_ERROR() << "Partition structure " << (unsigned) partition_type_ << " not supported";
}

/**
 * First we will do the local validations. Then we will call validation on the other objects
 */
void Model::Validate() {
  // Check that we've actually defined a @model block
  if (block_type_ == "")
    LOG_ERROR() << "The @model block is missing from configuration file. This block is required.";

  if (!parameters_.has_been_populated())
    parameters_.Populate(pointer());

  DoValidate();

  // Call validation for the other objects required by the model
  categories_->Validate();
  partition_->Validate();

  LOG_FINEST() << "Validating our managers now";
  managers_->Validate();
  LOG_FINEST() << "Finished validating managers";

  /**
   * Do some more sanity checks
   */
  initialisationphases::Manager& init_phase_mngr = *managers_->initialisation_phase();
  for (const string& phase : initialisation_phases_) {
    if (!init_phase_mngr.IsPhaseDefined(phase))
      LOG_ERROR_P(PARAM_INITIALISATION_PHASES) << "(" << phase << ") has not been defined. Please ensure you have defined it";
  }

  timesteps::Manager& time_step_mngr = *managers_->time_step();
  for (const string& time_step : time_steps_) {
    if (!time_step_mngr.GetTimeStep(time_step))
      LOG_ERROR_P(PARAM_TIME_STEPS) << "(" << time_step << ") has not been defined. Please ensure you have defined it";
  }

  if (parameters_.Get(PARAM_LENGTH_PLUS_GROUP)->has_been_defined() & (partition_type_ == PartitionType::kAge))
    LOG_ERROR_P(PARAM_LENGTH_PLUS_GROUP)
        << "This parameter should only be used for models that have length structured partitions. For age models with length based processes, all length bins should be defined by the length_bins subcommand";
}

/**
 *
 */
void Model::Build() {
  LOG_FINE() << "Model:Build()";
  // Set current year
  current_year_ = start_year_;

  categories()->Build();
  partition().Build();
  managers()->Build();

  Estimables& estimables = *managers_->estimables();
  if (estimables.GetValueCount() > 0) {
    addressable_values_file_  = true;
    addressable_values_count_ = estimables.GetValueCount();
  }

  managers_->Reset();
  LOG_FINE() << "Exit: Model:Build()";
}

/**
 *
 */
void Model::Verify() {
  LOG_TRACE();
  for (auto executor : executors_[state_]) executor->Execute();
}

/**
 *
 */
void Model::Reset() {
  LOG_TRACE();

  partition_->Reset();
  categories_->Reset();
  managers_->Reset();
}

/**
 *
 */
void Model::RunBasic() {
  LOG_TRACE();
  Estimables&     estimables  = *managers_->estimables();
  bool            single_step = global_configuration_->single_step();
  vector<string>  single_step_addressables;
  vector<Double*> estimable_targets;
  // Create an instance of all categories
  niwa::partition::accessors::All all_view(pointer());

  // Model is about to run
  for (unsigned i = 0; i < addressable_values_count_; ++i) {
    LOG_FINE() << "Model: State change to Initialise";
    state_        = State::kInitialise;
    current_year_ = start_year_;
    if (addressable_values_file_) {
      estimables.LoadValues(i);
      Reset();
    }

    /**
     * Running the model now
     */

    // Iterate over all partition members and UpDate Mean Weight for the inital weight calculations
    agelengths::Manager& age_length_manager = *managers_->age_length();    
    initialisationphases::Manager& init_phase_manager = *managers_->initialisation_phase();
    init_phase_manager.Execute();
    managers_->report()->Execute(pointer(), State::kInitialise);
    LOG_FINE() << "Model: State change to Execute";
    state_ = State::kExecute;

    /**
     * Handle single step now
     */
    if (single_step) {
      managers_->report()->Pause();
      cout << "Please enter a space separated list of addressable names to be used during single step" << endl;
      string line  = "";
      string error = "";

      std::getline(std::cin, line);
      managers_->report()->Resume();
      boost::split(single_step_addressables, line, boost::is_any_of(" "));
      for (string addressable : single_step_addressables) {
        if (!objects().VerifyAddressableForUse(addressable, addressable::kSingleStep, error)) {
          LOG_FATAL() << "The addressable " << addressable << " could not be verified for use in a single-step basic run. Error was " << error;
        }
        Double* target = objects().GetAddressable(addressable);
        estimable_targets.push_back(target);
      }
    }

    timesteps::Manager&   time_step_manager    = *managers_->time_step();
    timevarying::Manager& time_varying_manager = *managers_->time_varying();
    for (current_year_ = start_year_; current_year_ <= final_year_; ++current_year_) {
      age_length_manager.UpdateDataType(); // this only does something if we have data type age-length
      LOG_FINE() << "Iteration year: " << current_year_;
      if (single_step) {
        managers_->report()->Pause();
        cout << "Please enter space separated values for estimables for year: " << current_year_ << endl;
        string line = "";
        std::getline(std::cin, line);
        managers_->report()->Resume();

        vector<string> temp_values;
        boost::split(temp_values, line, boost::is_any_of(" "));
        if (temp_values.size() != estimable_targets.size()) {
          LOG_FATAL() << "Number of values provided was " << temp_values.size() << " when we expected " << estimable_targets.size();
        }

        for (unsigned i = 0; i < temp_values.size(); ++i) {
          Double value = 0;
          if (!utilities::To<string, Double>(temp_values[i], value)) {
            LOG_FATAL() << "Value " << temp_values[i] << " for the estimable " << single_step_addressables[i] << " is invalid";
          }

          LOG_FINEST() << "Setting annual value for " << single_step_addressables[i] << " to " << value;
          *estimable_targets[i] = value;
        }
      }
      LOG_TRACE();
      time_varying_manager.Update(current_year_);
      LOG_FINEST() << "finishing update time varying now Update Category mean length and weight before beginning annual cycle";
      time_step_manager.Execute(current_year_);
    }

    managers_->observation()->CalculateScores();

    for (auto executor : executors_[State::kExecute]) executor->Execute();

    // Model has finished so we can run finalise.

    LOG_FINE() << "Model: State change to Iteration Complete";
    objective_function_->CalculateScore();
    managers_->report()->Execute(pointer(), State::kIterationComplete);
  }
}

/**
 * Run the model in estimation mode.
 */
void Model::RunEstimation() {
  /*
   * Before running the model in estimation mode we'll do an iteration
   * as a basic model. We don't call any reports though.
   */
  LOG_FINE() << "Doing pre-estimation iteration of the model";
  Iterate();

  auto minimiser = managers_->minimiser()->active_minimiser();
  if (minimiser == nullptr)
    LOG_CODE_ERROR() << "if (minimiser == nullptr)";

  Estimables*         estimables = managers_->estimables();
  map<string, Double> estimable_values;
  LOG_FINE() << "estimable values count: " << addressable_values_count_;
  for (unsigned i = 0; i < addressable_values_count_; ++i) {
    if (addressable_values_file_) {
      estimables->LoadValues(i);
      Reset();
    }

    run_mode_ = RunMode::kEstimation;
    LOG_FINE() << "Calling minimiser to begin the estimation with the " << i + 1 << "st/nd/nth set of values";
    unsigned max_iters = managers_->estimate()->GetNumberOfPhases();

    LOG_FINE() << "found iterations = " << max_iters;
    for (unsigned j = 1; j <= max_iters; ++j) {
      LOG_MEDIUM() << "model.estimation_phase: " << j;
      managers_->estimate()->SetActivePhase(j);
      minimiser->Execute();
    }

    minimiser->BuildCovarianceMatrix();

    run_mode_ = RunMode::kBasic;
    FullIteration();
    run_mode_ = RunMode::kEstimation;

    LOG_FINE() << "Model: State change to Iteration Complete";
    managers_->report()->Execute(pointer(), State::kIterationComplete);
  }
}

/**
 *
 */

bool Model::RunMCMC() {
  LOG_CODE_ERROR() << "Deprecated";
  // LOG_FINE() << "Entering the MCMC Sub-System";
  // auto mcmc = managers_->mcmc()->active_mcmc();

  // Logging& logging = Logging::Instance();
  // if (logging.errors().size() > 0) {
  //   logging.FlushErrors();
  //   return false;
  // }

  // if (global_configuration_->resume_mcmc()) {
  //   configuration::MCMCObjective objective_loader(pointer());
  //   if (!objective_loader.LoadFile(global_configuration_->mcmc_objective_file()))
  //     return false;

  //   configuration::MCMCSample sample_loader(pointer());
  //   if (!sample_loader.LoadFile(global_configuration_->mcmc_sample_file()))
  //     return false;

  //   // reset RNG seed for resume
  //   utilities::RandomNumberGenerator::Instance().Reset((unsigned int)time(NULL));

  // } else if (!global_configuration_->skip_estimation()) {
  //   /**
  //    * Note: This should only be called when running Casal2 in a standalone executable
  //    * as it must use the same build profit (autodiff or not) as the MCMC. When
  //    * using the front end application, skip_estimation will be flagged as true.
  //    *
  //    * This is because the front end handles the minimisation to generate the MPD file
  //    * and Covariance matrix for use by the MCMC
  //    */
  //   LOG_FINE() << "Calling minimiser to find our minimum and covariance matrix";
  //   auto minimiser = managers_->minimiser()->active_minimiser();
  //   if ((minimiser->type() == PARAM_DE_SOLVER) | (minimiser->type() == PARAM_DLIB))
  //     LOG_ERROR() << "The minimiser type " << PARAM_DE_SOLVER << ", " << PARAM_DE_SOLVER
  //                 << " does not produce a covariance matrix and so will not be viable for an MCMC run, try one of the other minimisers.";

  //   minimiser->Execute();
  //   LOG_FINE() << "Build covariance matrix";
  //   minimiser->BuildCovarianceMatrix();
  //   LOG_FINE() << "Minimisation complete. Starting MCMC";
  // }
  // LOG_FINE() << "Begin MCMC chain";
  // // mcmc->Execute();
  return true;
}

/**
 *
 */
void Model::RunProfiling() {
  Estimables& estimables = *managers_->estimables();

  map<string, Double> estimable_values;
  for (unsigned i = 0; i < addressable_values_count_; ++i) {
    if (addressable_values_file_) {
      estimables.LoadValues(i);
      Reset();
    }

    LOG_FINE() << "Doing pre-profile iteration of the model";
    Iterate();

    LOG_FINE() << "Entering the Profiling Sub-System";
    estimates::Manager& estimate_manager = *managers_->estimate();
    auto                minimiser        = managers_->minimiser()->active_minimiser();
    if (!minimiser)
      LOG_FATAL() << "couldn't get an active minimiser to estimate for the profile";
    vector<Profile*> profiles = managers_->profile()->objects();
    LOG_INFO() << "Profiling with " << profiles.size() << " parameter(s)";
    for (auto profile : profiles) {
      LOG_INFO() << "Profiling parameter " << profile->parameter();
      LOG_FINE() << "Disabling estimate: " << profile->parameter();
      estimate_manager.UnFlagIsEstimated(profile->parameter());

      LOG_FINE() << "First-Stepping profile";
      profile->FirstStep();
      for (unsigned i = 0; i < profile->steps(); ++i) {
        LOG_FINE() << "Calling minimiser to begin the estimation (profiling)";
        LOG_INFO() << "Profiling with parameter at step " << i + 1 << " of " << profile->steps() << " steps";

        minimiser->Execute();
        LOG_FINE() << "Finished estimation from " << i + 1 << " steps";
        run_mode_ = RunMode::kBasic;
        FullIteration();

        LOG_FINE() << "Model: State change to Iteration Complete";
        run_mode_ = RunMode::kProfiling;
        managers_->report()->Execute(pointer(), State::kIterationComplete);

        profile->NextStep();
      }
      profile->RestoreOriginalValue();

      estimate_manager.FlagIsEstimated(profile->parameter());
    }
  }
}

/**
 *
 */
void Model::RunSimulation() {
  LOG_FINE() << "Entering the Simulation Sub-System";

  Estimables* estimables = managers_->estimables();
  LOG_FINE() << "estimable values count: " << addressable_values_count_;

  niwa::partition::accessors::All all_view(pointer());

  int simulation_candidates = global_configuration_->simulation_candidates();
  if (simulation_candidates < 1) {
    LOG_FATAL() << "The number of simulations specified at the command line parser must be at least one";
  }
  // Suffix for sims
  unsigned first_suffix_width = (unsigned)(floor(log10((double)(addressable_values_count_))) + 1);
  unsigned second_suffix_width = (unsigned)(floor(log10((double)(simulation_candidates))) + 1);
  unsigned s_width;
  unsigned i_width;
  unsigned init_diff;
  unsigned second_diff;
  for (unsigned i = 0; i < addressable_values_count_; ++i) {
    LOG_FINE() << "addressable i = " << i + 1;
    LOG_FINE() << "Model: State change to Initialise";
    state_        = State::kInitialise;
    current_year_ = start_year_;
    i_width = (unsigned)(floor(log10((i + 1))) + 1);
    init_diff = first_suffix_width - i_width;
    // set addressables
    if (addressable_values_file_) {
      estimables->LoadValues(i);
      Reset();
    }
    initialisationphases::Manager& init_phase_manager = *managers_->initialisation_phase();
    init_phase_manager.Execute();
    managers_->report()->Execute(pointer(), State::kInitialise);
    LOG_FINE() << "Model: State change to Execute";
    state_                                     = State::kExecute;
    timesteps::Manager&   time_step_manager    = *managers_->time_step();
    timevarying::Manager& time_varying_manager = *managers_->time_varying();
    agelengths::Manager& age_length_manager = *managers_->age_length();    

    for (current_year_ = start_year_; current_year_ <= final_year_; ++current_year_) {
      LOG_FINE() << "Iteration year: " << current_year_;
      age_length_manager.UpdateDataType(); // this only does something if we have data type age-length
      time_varying_manager.Update(current_year_);
      managers_->simulate()->Update(current_year_);
      time_step_manager.Execute(current_year_);
    }
    // model finish running given this set of -i
    for (int s = 0; s < simulation_candidates; ++s) {
      LOG_FINE() << "simulation s = " << s;
      string report_suffix = ".";
      s_width = (unsigned)(floor(log10((s + 1))) + 1);
      second_diff = second_suffix_width - s_width;
      report_suffix.append(init_diff, '0');
      report_suffix.append(utilities::ToInline<unsigned, string>(i + 1));
      report_suffix.append(1, '_');
      report_suffix.append(second_diff, '0');
      report_suffix.append(utilities::ToInline<unsigned, string>(s + 1));
      managers_->report()->set_report_suffix(report_suffix);
      // This function will also simulate observations
      managers_->observation()->CalculateScores();
      // Model has finished so we can run finalise.
      LOG_FINE() << "Model: State change to PostExecute";
      managers_->report()->Execute(pointer(), State::kIterationComplete);

      managers_->report()->WaitForReportsToFinish();
    }
  }
}

/**
 *
 */

void Model::RunProjection() {
  LOG_TRACE();
  int projection_candidates = global_configuration_->projection_candidates();
  if (projection_candidates < 1) {
    LOG_FATAL() << "The number of projections specified at the command line parser must be at least one";
  }
  Estimables&     estimables = *managers_->estimables();
  vector<Double*> estimable_targets;
  // Create an instance of all categories
  niwa::partition::accessors::All all_view(pointer());
  agelengths::Manager& age_length_manager = *managers_->age_length();    
  // Model is about to run
  for (unsigned i = 0; i < addressable_values_count_; ++i) {
    for (int j = 0; j < projection_candidates; ++j) {
      LOG_FINE() << "Beginning initial model run for projections";
      projection_final_phase_ = false;
      if (addressable_values_file_) {
        LOG_FINE() << "loading input parameters";
        estimables.LoadValues(i);
        Reset();
      }

      LOG_FINE() << "Model: State change to Execute";
      state_        = State::kInitialise;
      current_year_ = start_year_;
      initialisationphases::Manager& init_phase_manager = *managers_->initialisation_phase();
      // update data type needs to be updated in the f loop
      age_length_manager.UpdateDataType();
      init_phase_manager.Execute();

      state_ = State::kExecute;

      timesteps::Manager&   time_step_manager    = *managers_->time_step();
      timevarying::Manager& time_varying_manager = *managers_->time_varying();
      projects::Manager&    project_manager      = *managers_->project();

      for (current_year_ = start_year_; current_year_ <= final_year_; ++current_year_) {
        LOG_FINE() << "Iteration year: " << current_year_;
        age_length_manager.UpdateDataType(); // this only does something if we have data type age-length
        time_varying_manager.Update(current_year_);
        time_step_manager.Execute(current_year_);
        project_manager.StoreValues(current_year_);
      }

      /**
       * Running the model now
       */
      LOG_FINE() << "Entering the Projection Sub-System";
      // Reset the model
      projection_final_phase_ = true;
      Reset();
      state_        = State::kInitialise;
      current_year_ = start_year_;
      // Run the intialisation phase
      init_phase_manager.Execute();
      // Reset all parameter and re run the model
      managers_->report()->Execute(pointer(), State::kInitialise);

      state_ = State::kExecute;
      LOG_FINE() << "Starting projection years";
      for (; current_year_ <= projection_final_year_; ++current_year_) {
        LOG_FINE() << "Iteration year: " << current_year_;
        project_manager.Update(current_year_);
        time_step_manager.Execute(current_year_);
      }

      // Model has finished so we can run finalise.
      LOG_FINE() << "Model: State change to PostExecute and iteration complete";
      managers_->report()->Execute(pointer(), State::kIterationComplete);

      // Not sure if we need these
      // managers_->observation()->CalculateScores();
      // managers_->report()->WaitForReportsToFinish();
      // Reset();
    }
  }
  // Print the report to disk if tabular
  // managers_->report()->Finalise();
}

/**
 * This method will do a single iteration of the model. During
 * a basic run it'll only run once, but during the other run modes i.e. estimation and MCMC
 * it'll run multiple times.
 */
void Model::Iterate() {
  LOG_TRACE();
  // Create an instance of all categories
  niwa::partition::accessors::All all_view(pointer());

  state_        = State::kInitialise;
  current_year_ = start_year_; 
  agelengths::Manager& age_length_manager = *managers_->age_length();    
  initialisationphases::Manager& init_phase_manager = *managers_->initialisation_phase();
  // update data type before Init phase
  age_length_manager.UpdateDataType(); 
  init_phase_manager.Execute();
  managers_->report()->Execute(pointer(), State::kInitialise);

  state_                                     = State::kExecute;
  timesteps::Manager&   time_step_manager    = *managers_->time_step();
  timevarying::Manager& time_varying_manager = *managers_->time_varying();
  for (current_year_ = start_year_; current_year_ <= final_year_; ++current_year_) {
    age_length_manager.UpdateDataType(); // this only does something if we have data type age-length
    LOG_FINE() << "Iteration year: " << current_year_;
    time_varying_manager.Update(current_year_);

    time_step_manager.Execute(current_year_);
  }

  managers_->observation()->CalculateScores();

  for (auto executor : executors_[State::kExecute]) executor->Execute();

  current_year_ = final_year_;
}

/**
 *
 */
void Model::FullIteration() {
  // TODO: Add calculation of the objective score here and estimate transformations
  Reset();
  Iterate();
}
} /* namespace niwa */
