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
 * that creates the models and hands them off to the different objects for running (minimisers, mcmcs etc)
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

#include "BaseClasses/Object.h"
#include "ConfigurationLoader/Loader.h"
#include "Estimables/Estimables.h"
#include "Estimates/Manager.h"
#include "Minimisers/Manager.h"
#include "Minimisers/Minimiser.h"
#include "Model/Factory.h"
#include "Model/Managers.h"
#include "Model/Models/Age.h"
#include "Reports/Manager.h"
#include "Utilities/RandomNumberGenerator.h"
#include "Utilities/StandardHeader.h"

// namespaces
namespace niwa {
using std::cout;
using std::endl;
using std::shared_ptr;
using std::string;

/**
 * Default constructor
 */
Runner::Runner() {}

/**
 * Destructor
 */
Runner::~Runner() {}

/**
 * This is the main entry point for our Casal2 system. This is called from the main() and shared library methods.
 *
 * Note: We're not using a switch here because we want to have some logical separation of the
 * different run modes in our code.
 */
int Runner::Go() {
  RunMode::Type run_mode    = run_parameters_.run_mode_;
  int           return_code = 0;

  /**
   * Handle the run modes where we want to quick exit
   */
  if (run_mode == RunMode::kInvalid) {
    LOG_CODE_ERROR() << "The current run_mode is kInvalid, this is a bug";
  }
  if (run_mode == RunMode::kUnitTest) {
    LOG_ERROR() << "The Unit Test run mode is not supported in this binary. Please ensure you're running it correctly";
    return -1;
  }
  if (run_mode == RunMode::kVersion || run_mode == RunMode::kHelp || run_mode == RunMode::kLicense) {
    return 0;
  }

  // Handle our Query run mode
  if (run_mode == RunMode::kQuery) {
    return RunQuery() ? 0 : -1;
  }

  /**
   * Now we're getting into the different run modes that will execute the model
   * in some form.
   */
  Logging &logging = Logging::Instance();

  // print the start of our standard header
  utilities::StandardHeader standard_header;
  standard_header.PrintTop(global_configuration_);

  // load our configuration file
  configuration::Loader config_loader;
  if (!config_loader.LoadFromDiskToMemory(global_configuration_)) {
    logging.FlushErrors();
    return false;
  }

  // LOG_FATAL() << "Defined model type is: " << config_loader.model_type();
  // LOG_FATAL() << "Active minimiser type is " << config_loader.minimiser_type();

  master_model_ = Factory::Create(PARAM_MODEL, config_loader.model_type());
  master_model_->flag_primary_thread_model();
  // Create the managers that are shared across threads
  auto reports_manager = shared_ptr<reports::Manager>(new reports::Manager());
  master_model_->managers()->set_reports(reports_manager);
  auto minimiser_manager = shared_ptr<minimisers::Manager>(new minimisers::Manager());
  master_model_->managers()->set_minimiser(minimiser_manager);

  master_model_->set_run_mode(run_mode);

  // Grab our model type as specified in the config file.
  // If it's not the type we created by default, create
  // a new master model
  string model_type = config_loader.model_type();
  if (master_model_->type() != model_type) {
    master_model_->factory().Create(PARAM_MODEL, model_type);
    master_model_->flag_primary_thread_model();
  }

  // Next we'll parse our configuration file into the master model
  // We do this separately to the others so we can grab the number of threads needed
  vector<shared_ptr<Model>> model_list;
  model_list.push_back(master_model_);
  master_model_->set_id(1);
  config_loader.Build(model_list);
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
    model->managers()->set_reports(reports_manager);
    model->managers()->set_minimiser(minimiser_manager);
    model->set_run_mode(run_mode);
    model_list.push_back(model);
  }

  // If we're running +1 threads, build our new model_list
  if (model_list.size() > 0)
    config_loader.Build(model_list);

  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return -1;
  }

  // Put the master model back into the list at the start
  //	model_list[0]->flag_primary_thread_model();
  model_list.insert(model_list.begin(), master_model_);

  // override any config file values from our command line parameters
  master_model_->global_configuration().ParseOptions(master_model_);
  master_model_->global_configuration().set_run_parameters(run_parameters_);  // TODO: Set global_configuration for models too from Runner
  utilities::RandomNumberGenerator::Instance().Reset(master_model_->global_configuration().random_seed());

  // Thread off the reports
  // Must be done before we validate and build below
  std::thread report_thread([reports_manager]() { reports_manager->FlushReports(); });

  /**
   * Prep each of the models for being run
   * i.e. Validate and Build them
   */
  for (auto model : model_list) {
    if (!model->PrepareForIterations()) {
      if (logging.errors().size() > 0) {
        logging.FlushErrors();
        return -1;
      }
    }
  }

  thread_pool_.reset(new ThreadPool());
  thread_pool_->CreateThreads(model_list);
  //	thread_pool_->CheckThreads();

  /**
   * Check the run mode and call the handler.
   */
  switch (run_mode) {
    case RunMode::kBasic:
      master_model_->Start(run_mode);
      break;
    case RunMode::kEstimation:
      return_code = RunEstimation() ? 0 : -1;
      break;
    case RunMode::kMCMC:
      master_model_->Start(run_mode);
      break;
    case RunMode::kSimulation:
      master_model_->Start(run_mode);
      break;
    case RunMode::kProfiling:
      master_model_->Start(run_mode);
      break;
    case RunMode::kProjection:
      master_model_->Start(run_mode);
      break;
    case RunMode::kTesting:
      master_model_->Start(run_mode);
      break;
    default:
      break;
  }

  // we're finished. Terminate our threads to cleanup memory
  thread_pool_->TerminateAll();

  // finish report thread
  reports_manager->StopThread();
  report_thread.join();

  // check for any errors
  if (logging.errors().size() > 0) {
    logging.FlushErrors();
    return_code = -1;
  } else
    logging.FlushWarnings();

  standard_header.PrintBottom(global_configuration_);

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
  string         lookup = global_configuration_.object_to_query();
  vector<string> parts;
  boost::split(parts, lookup, boost::is_any_of("."));

  if (parts.size() == 1)
    parts.push_back("");

  if (parts.size() == 2) {
    master_model_->set_partition_type(PartitionType::kAge);
    base::Object *object = master_model_->factory().CreateObject(parts[0], parts[1], PartitionType::kModel);
    if (object) {
      cout << "Printing information for " << parts[0] << " with sub-type " << parts[1] << endl;
      object->PrintParameterQueryInfo();
    } else {
      master_model_->set_partition_type(PartitionType::kLength);
      object = master_model_->factory().CreateObject(parts[0], parts[1], PartitionType::kModel);
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

  auto                estimables_manager = managers->estimables();
  map<string, Double> estimable_values;
  bool                use_addressable_file = master_model_->addressables_value_file();
  unsigned            iterations_to_do     = estimables_manager->GetValueCount() == 0 ? 1 : estimables_manager->GetValueCount();
  for (unsigned i = 0; i < iterations_to_do; ++i) {
    if (use_addressable_file) {
      estimables_manager->LoadValues(i);
      //			master_model_->Reset();
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

  LOG_MEDIUM() << "Model: State change to Finalise";
  master_model_->Finalise();
  return true;
}

} /* namespace niwa */
