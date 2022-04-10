/**
 * @file Runner.cpp
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 20/09/2019
 * @section LICENSE
 *
 * Copyright NIWA Science �2019 - www.niwa.co.nz
 *
 * @description
 * This class is the class that is spawned from main() and holds pointers to everything. It's the class
 * that creates the models and hands them off to the different objects for running (minimisers, MCMCs etc.)
 *
 */

// headers
#include "Runner.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <iostream>
#include <memory>
#include <string>

#include "AddressableInputLoader/AddressableInputLoader.h"
#include "AddressableTransformations/Manager.h"
#include "AgeLengths/Manager.h"
#include "BaseClasses/Object.h"
#include "ConfigurationLoader/AddressableInputValueLoader.h"
#include "ConfigurationLoader/MCMCObjective.h"
#include "ConfigurationLoader/MCMCSample.h"
#include "Estimates/Manager.h"
#include "MCMCs/Manager.h"
#include "Minimisers/Manager.h"
#include "Minimisers/Minimiser.h"
#include "Model/Factory.h"
#include "Model/Managers.h"
#include "Model/Models/Age.h"
#include "Profiles/Manager.h"
#include "Utilities/RandomNumberGenerator.h"

// namespaces
namespace niwa {
using std::cout;
using std::endl;
using std::shared_ptr;
using std::string;

/**
 * @brief Construct a new Runner:: Runner object
 *
 */
Runner::Runner() {
  reports_manager_.reset(new reports::Manager());
}

/**
 * @brief Destroy the Runner:: Runner object
 *
 */
Runner::~Runner() {
  if (reports_manager_)
    reports_manager_->StopThread();
  if (report_thread_ && report_thread_->joinable())
    report_thread_->join();
}

/**
 * @brief The startup method is responsible for ensuring the command
 * line parameters are parsed and loading the
 * configuration files and initialising pointers to member
 * objects that are shared across the system.
 *
 * @return true (continue)
 * @return false (failure)
 */
bool Runner::StartUp() {
  // Stage 1 - Handle run modes that are not a model iteration
  switch (run_mode_) {
    case RunMode::kInvalid:
      LOG_CODE_ERROR() << "The current run_mode is kInvalid, this is a bug. Please report this error to the Casal2 Development Team";
      break;
    case RunMode::kUnitTest:
      LOG_ERROR() << "The Unit Test run mode is not supported in this binary. Please ensure you're running it correctly";
      return false;
    case RunMode::kQuery:
      return RunQuery();
    default:
      break;
  }

  // Stage 1 - Parse the command line parameters
  // override any config file values from our command line parameters
  global_configuration_.set_run_parameters(run_parameters_);

  // Stage 2 - Load configuration file information
  if (!config_loader_.LoadFromDiskToMemory(global_configuration_)) {
    Logging::Instance().FlushErrors();
    return false;
  }

  // Stage 3 - Parse the options loaded so we can ensure those
  // loaded from the configuration file are set and then
  // we override them from the command line
  global_configuration_.ParseOptions();

  // Reset RNG
  utilities::RandomNumberGenerator::Instance().Reset(global_configuration_.random_seed());

  return true;
}

bool Runner::Validate() {
  return true;
}
bool Runner::Build() {
  return true;
}
bool Runner::Verify() {
  return true;
}
bool Runner::Execute() {
  return true;
}

/**
 * @brief Execution of the chosen run mode has finished. So this will
 * clean up any assets we've created and shut down the threads
 *
 * @return true
 * @return false
 */
bool Runner::Finalize() {
  if (run_mode_ != RunMode::kTesting) {
    // we're finished. Terminate our threads to cleanup memory
    LOG_MEDIUM() << "Terminating threads (not in Test run mode: " << run_mode_ << ")";
    thread_pool_->TerminateAll();
  }
  return true;
}

/**
 * This is a default override of the Go() method. We use this when we have already
 * set the run mode via the run_parameters.
 */
int Runner::Go() {
  RunMode::Type run_mode = run_parameters_.run_mode_;
  return GoWithRunMode(run_mode);
}

/**
 * @brief This is the main entry point for our Casal2 system. This is called from the main() and shared library methods.
 *
 * Note: We're not using a switch here because we want to have some logical separation of the
 * different run modes in our code.
 *
 * @param run_mode
 * @return int
 */
int Runner::GoWithRunMode(RunMode::Type run_mode) {
  LOG_TRACE();
  run_parameters_.run_mode_ = run_mode;
  run_mode_                 = run_mode;
  int return_code           = 0;

  // These run modes are handled elsewhere. TODO: Move them here somewhere
  if (run_mode == RunMode::kVersion || run_mode == RunMode::kHelp || run_mode == RunMode::kLicense) {
    return 0;
  }

  if (!StartUp())
    return -1;
  if (run_mode == RunMode::kQuery)
    return 0;

  /**
   * Now we're getting into the different run modes that will execute the model
   * in some form.
   */
  Logging& logging = Logging::Instance();

  // load our configuration file
  // configuration::Loader config_loader;

  // Grab our model type as specified in the config file.
  // create a new master model
  string model_type = config_loader_.model_type();
  master_model_     = Factory::Create(PARAM_MODEL, model_type);
  master_model_->flag_primary_thread_model();
  master_model_->set_global_configuration(&global_configuration_);

  // Create the managers that are shared across threads
  master_model_->managers()->set_reports(reports_manager_);
  auto minimiser_manager = shared_ptr<minimisers::Manager>(new minimisers::Manager());
  master_model_->managers()->set_minimiser(minimiser_manager);
  auto mcmc_manager = std::make_shared<mcmcs::Manager>();
  master_model_->managers()->set_mcmc(mcmc_manager);
  mpd_ = std::make_shared<niwa::MPD>(master_model_);

  master_model_->set_run_mode(run_mode);

  // Next we'll parse our configuration file into the master model
  // We do this separately to the others so we can grab the number of threads needed
  vector<shared_ptr<Model>> model_list;
  model_list.push_back(master_model_);
  master_model_->set_id(1);
  config_loader_.Build(model_list);
  model_list.clear();  // so we don't re-parse configuration file into it
  unsigned thread_count = master_model_->threads();

  /**
   * Now we spawn our threads and populate the models
   */
  LOG_MEDIUM() << "number of threads specified for model: " << master_model_->threads();
  for (unsigned i = 1; i < thread_count; ++i) {
    LOG_MEDIUM() << "Spawning model for thread with id " << (i + 1);
    shared_ptr<Model> model = Factory::Create(PARAM_MODEL, model_type);
    model->set_id(i + 1);
    model->managers()->set_reports(reports_manager_);
    model->managers()->set_minimiser(minimiser_manager);
    model->set_global_configuration(&global_configuration_);
    model->set_run_mode(run_mode);
    model_list.push_back(model);
  }

  // If we're running +1 threads, build our new model_list
  if (model_list.size() > 0)
    config_loader_.Build(model_list);

  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return -1;
  }

  // Put the master model back into the list at the start
  //	model_list[0]->flag_primary_thread_model();
  model_list.insert(model_list.begin(), master_model_);
  if (print_reports_)
    report_thread_.reset(new std::thread([this]() { this->reports_manager_->FlushReports(); }));

  /**
   * Prep each of the models for being run
   * i.e. Validate and Build them
   */
  for (auto model : model_list) {
    if (!model->PrepareForIterations()) {
      logging.FlushErrors();
      logging.FlushVerifies();
      return -1;
    }
  }

  // Kick off our threads now
  thread_pool_.reset(new ThreadPool());
  thread_pool_->CreateThreads(model_list);

  /**
   * Check the run mode and call the handler.
   */
  switch (run_mode) {
    case RunMode::kBasic:
      LOG_INFO() << "Initiating basic run mode";
      master_model_->Start(run_mode);
      break;
    case RunMode::kEstimation:
      LOG_INFO() << "Initiating point estimate run mode";
      return_code = RunEstimation() ? 0 : -1;
      break;
    case RunMode::kMCMC:
      LOG_INFO() << "Initiating MCMC run mode";
      return_code = RunMCMC();
      break;
    case RunMode::kSimulation:
      LOG_INFO() << "Initiating simulation run mode";
      master_model_->Start(run_mode);
      break;
    case RunMode::kProfiling:
      LOG_INFO() << "Initiating profile run mode";
      // master_model_->Start(run_mode);
      return_code = RunProfiling() ? 0 : -1;
      break;
    case RunMode::kProjection:
      LOG_INFO() << "Initiating projection run mode";
      master_model_->Start(run_mode);
      break;
    case RunMode::kTesting:
      LOG_INFO() << "Initiating test mode";
      master_model_->Start(run_mode);
      break;
    default:
      break;
  }

  Finalize();

  // finish report thread
  if (reports_manager_)
    reports_manager_->StopThread();
  if (report_thread_)
    report_thread_->join();

  // check for any errors
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return_code = -1;
  } else {
    logging.FlushInfo();
    logging.FlushImportant();
    logging.FlushWarnings();
  }

  return return_code;
}

/**
 * This method handles the --query command line parameter. A user is able to query an object
 * to see what the parameters are that it supports in the configuration file by doing something like
 *
 * casal2 --query=process.recruitment_constant
 *
 * @return true on success, false on failure
 */
bool Runner::RunQuery() {
  LOG_TRACE();
  string         lookup = global_configuration_.object_to_query();
  vector<string> parts;
  boost::split(parts, lookup, boost::is_any_of("."));

  if (parts.size() == 1)
    parts.push_back("");

  if (parts.size() == 2) {
    master_model_        = Factory::Create(PARAM_MODEL, PARAM_AGE);
    base::Object* object = master_model_->factory().CreateObject(parts[0], parts[1], PartitionType::kModel);
    if (object) {
      cout << "Printing information for " << parts[0] << " with sub-type " << parts[1] << endl;
      object->PrintParameterQueryInfo();
    } else {
      master_model_ = Factory::Create(PARAM_MODEL, PARAM_LENGTH);
      object        = master_model_->factory().CreateObject(parts[0], parts[1], PartitionType::kModel);
      if (object) {
        cout << "Printing information for " << parts[0] << " with sub-type " << parts[1] << endl;
        object->PrintParameterQueryInfo();
      } else {
        cout << "Object type " << lookup << " is invalid" << endl;
        return false;
      }
    }

  } else {
    cout << "Please use object_type.sub_type only when querying an object" << endl;
    return false;
  }

  return true;
}

/**
 *
 */
bool Runner::RunEstimation() {
  /*
   * Before running the model in estimation mode we'll do an iteration
   * as a basic model. We don't call any reports though.
   */
  LOG_MEDIUM() << "Doing pre-estimation iteration of the model";
  master_model_->set_run_mode(RunMode::kEstimation);
  master_model_->FullIteration();

  auto managers  = master_model_->managers();
  auto minimiser = managers->minimiser()->active_minimiser();
  if (minimiser == nullptr)
    LOG_CODE_ERROR() << "if (minimiser == nullptr)";

  AddressableInputLoader* addressable_input_loader = master_model_->managers()->addressable_input_loader();
  bool                    use_addressable_file     = master_model_->addressables_value_file();
  unsigned                iterations_to_do         = addressable_input_loader->GetValueCount() == 0 ? 1 : addressable_input_loader->GetValueCount();
  for (unsigned i = 0; i < iterations_to_do; ++i) {
    if (use_addressable_file) {
      addressable_input_loader->LoadValues(i);
    }

    master_model_->set_run_mode(RunMode::kEstimation);
    LOG_MEDIUM() << "Calling minimiser to begin the estimation with the " << i + 1 << "st/nd/nth set of values";

    unsigned phases = managers->estimate()->GetNumberOfPhases();
    for (unsigned j = 1; j <= phases; ++j) {
      LOG_MEDIUM() << "model.estimation_phase: " << j;
      managers->estimate()->SetActivePhase(j);
      minimiser->ExecuteThreaded(thread_pool_);
    }

    minimiser->BuildCovarianceMatrix();

    master_model_->set_run_mode(RunMode::kBasic);
    master_model_->FullIteration();

    master_model_->set_run_mode(RunMode::kEstimation);
    LOG_MEDIUM() << "Model: State change to Iteration Complete";
    managers->report()->Execute(master_model_, State::kIterationComplete);
  }

  // Create and store our MPD so it can go back via the SharedLibrary
  if (global_configuration_.estimation_is_for_mcmc()) {
    mpd_->CreateMPD(master_model_);
    run_parameters_.mpd_data_ = mpd_->value();
  }

  LOG_MEDIUM() << "Model: State change to Finalise";
  master_model_->Finalise();
  return true;
}

/**
 * @brief Run the system through a markov-chain monte-carlo
 *
 * @return int Return code
 */
int Runner::RunMCMC() {
  LOG_FINE() << "Entering the MCMC Sub-System";
  auto mcmc = master_model_->managers()->mcmc()->active_mcmc();
  if (!mcmc)
    LOG_CODE_ERROR() << "!mcmc";

  Logging& logging = Logging::Instance();
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return false;
  }

  /**
   * @brief Load our MPD file if one exists so that we can
   * set the estimate and covariance matrix to right values
   * But don't do this if we're not resuming
   *
   * If covariance size has already been set, we've obviously
   * loaded it from somewhere else (e.g. unit test)
   */
  auto config = global_configuration();
  if (run_parameters_.mpd_data_ != "") {
    mpd_->ParseString(run_parameters_.mpd_data_);

  } else if (config.resume_mcmc() || config.get_mpd_input_file() != "" || !config.estimate_before_mcmc()) {
    if (config.estimate_before_mcmc())
      LOG_INFO() << "Initiating point estimate run mode before MCMC";

    string mpd_file_name = config.get_mpd_input_file();

    if (mpd_file_name == "") {
      LOG_FATAL() << "You must specify the MPD file if you're resuming an MCMC or skipping the pre-MCMC estimation";
    }

    if (!mpd_->LoadFromDiskToMemory(mpd_file_name)) {
      LOG_FATAL() << "Failed to load MPD values and covariance from " << mpd_file_name << " file";
    }
  }

  // TODO: Replace with call to mcmc::Manager.Resume();
  // TODO: Support chaining MCMC algorithms?
  if (global_configuration_.resume_mcmc()) {
    LOG_INFO() << "Resuming MCMC";
    configuration::MCMCObjective objective_loader(master_model_);
    if (!objective_loader.LoadFile(global_configuration_.mcmc_objective_file()))
      return false;

    configuration::MCMCSample sample_loader(master_model_);
    if (!sample_loader.LoadFile(global_configuration_.mcmc_sample_file()))
      return false;

    // reset RNG seed for resume
    utilities::RandomNumberGenerator::Instance().Reset((unsigned int)time(NULL));

  } else if (global_configuration_.estimate_before_mcmc()) {
    /**
     * Note: This should only be called when running Casal2 in a standalone executable
     * as it must use the same build profile (autodiff or not) as the MCMC. When
     * using the front end application, estimate_before_mcmc will be flagged as true.
     * This is because the front end handles the minimisation to generate the MPD file
     * and Covariance matrix for use by the MCMC
     */
    LOG_FINE() << "Calling minimiser to find our minimum and covariance matrix";
    auto minimiser = master_model_->managers()->minimiser()->active_minimiser();
    if (!minimiser) {
      LOG_CODE_ERROR() << "!minimiser";
    }
    if (minimiser->type() == PARAM_DE_SOLVER)
      LOG_ERROR() << "The minimiser type " << PARAM_DE_SOLVER
                  << " does not produce a covariance matrix and so will not be viable for an MCMC run, try one of the other minimisers.";

    LOG_FINE() << "Calling Minimiser with thread_pool";
    minimiser->ExecuteThreaded(thread_pool_);
    LOG_FINE() << "Build covariance matrix";
    minimiser->BuildCovarianceMatrix();
    LOG_FINE() << "Minimisation complete. Starting MCMC";

    // TODO: Implement a setter here
    mcmc->covariance_matrix() = minimiser->covariance_matrix();
    auto covariance_matrix_   = minimiser->covariance_matrix();
    LOG_MEDIUM() << "Printing Covariance Matrix from Minimiser";
    for (unsigned i = 0; i < covariance_matrix_.size1(); ++i) {
      for (unsigned j = 0; j < covariance_matrix_.size2(); ++j) {
        LOG_MEDIUM() << "row = " << i + 1 << " col = " << j + 1 << " value = " << covariance_matrix_(i, j);
      }
    }
  }  // else if (global_configuration_.estimate_before_mcmc())

  LOG_INFO() << "Beginning MCMC iterations";
  mcmc->Execute(thread_pool_);
  LOG_MEDIUM() << "Model MCMC: State change to Finalise";
  master_model_->Finalise();
  return 0;
}

/**
 * Run a profiles
 */
bool Runner::RunProfiling() {
  /*
   * Before running the model in estimation mode we'll do an iteration
   * as a basic model. We don't call any reports though.
   */
  LOG_MEDIUM() << "Doing pre-profiling iteration of the model";
  master_model_->set_run_mode(RunMode::kProfiling);
  master_model_->FullIteration();
  auto managers  = master_model_->managers();
  auto minimiser = managers->minimiser()->active_minimiser();

  if (minimiser == nullptr)
    LOG_CODE_ERROR() << "if (minimiser == nullptr)";

  AddressableInputLoader* addressable_input_loader = managers->addressable_input_loader();

  bool     use_addressable_file = master_model_->addressables_value_file();
  unsigned iterations_to_do     = addressable_input_loader->GetValueCount() == 0 ? 1 : addressable_input_loader->GetValueCount();
  // for each -i
  for (unsigned i = 0; i < iterations_to_do; ++i) {
    if (use_addressable_file) {
      addressable_input_loader->LoadValues(i);
    }

    master_model_->set_run_mode(RunMode::kProfiling);
    LOG_MEDIUM() << "About to begin profile for the " << i + 1 << "st/nd/nth set of values";
    vector<Profile*> profiles = managers->profile()->objects();
    LOG_INFO() << "Profiling with " << profiles.size() << " parameter(s)";
    for (auto profile : profiles) {
      LOG_INFO() << "Profiling parameter " << profile->parameter();
      LOG_FINE() << "Disabling estimate: " << profile->parameter();
      managers->estimate()->UnFlagIsEstimated(profile->parameter());
      LOG_FINE() << "First-Stepping profile";
      profile->FirstStep();
      for (unsigned j = 0; j < profile->steps(); ++j) {
        LOG_FINE() << "Calling minimiser to begin the estimation (profiling)";
        LOG_INFO() << "Profiling with parameter at step " << j + 1 << " of " << profile->steps() << " steps";
        minimiser->ExecuteThreaded(thread_pool_);
        LOG_FINE() << "Finished estimation from " << j + 1 << " steps";
        master_model_->set_run_mode(RunMode::kBasic);
        master_model_->FullIteration();
        master_model_->set_run_mode(RunMode::kProfiling);
        LOG_FINE() << "Model: State change to Iteration Complete";
        managers->report()->Execute(master_model_, State::kIterationComplete);
        LOG_FINE() << "j = " << j << " steps = " << profile->steps();
        if(j != (profile->steps() - 1)) // we don't estimate the last step!
          profile->NextStep();
        LOG_FINE() << "j = " << j << " steps = " << profile->steps();
      }
      profile->RestoreOriginalValue();
      managers->estimate()->FlagIsEstimated(profile->parameter());
    }
  }
  LOG_MEDIUM() << "Model: State change to Finalise";
  master_model_->Finalise();
  return true;
}

/**
 * @brief
 *
 * @return shared_ptr<Model>
 */
shared_ptr<Model> Runner::model() {
  if (!master_model_)
    LOG_CODE_ERROR() << "!master_model_";
  return master_model_;
}

/**
 * @brief
 *
 * @return shared_ptr<ThreadPool>
 */
shared_ptr<ThreadPool> Runner::thread_pool() {
  return thread_pool_;
}

/**
 * @brief
 *
 * @return shared_ptr<MPD>
 */
shared_ptr<MPD> Runner::mpd() {
  if (!mpd_)
    LOG_CODE_ERROR() << "!mpd_";
  return mpd_;
}

} /* namespace niwa */
