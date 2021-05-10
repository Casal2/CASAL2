/**
 * @file Runner.h
 * @author  Scott Rasmussen (scott@zaita.com)
 * @date 20/09/2019
 * @section LICENSE
 *
 * Copyright NIWA Science �2019 - www.niwa.co.nz
 */
#ifndef SOURCE_RUNNER_H_
#define SOURCE_RUNNER_H_

// headers
#include "ConfigurationLoader/Loader.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "Model/Model.h"
#include "ThreadPool/ThreadPool.h"
#include "Utilities/StandardHeader.h"

// namespaces
namespace niwa {

// class declaration
class Runner {
  friend class BaseThreaed;  // Unit test base model
public:
  // methods
  Runner()          = default;
  virtual ~Runner() = default;

  bool StartUp();
  bool Validate();
  bool Build();
  bool Verify();
  bool Execute();
  bool Finalize();

  int                    Go();
  int                    GoWithRunMode(RunMode::Type run_mode);
  void                   set_run_parameters(utilities::RunParameters& options) { run_parameters_ = options; }
  GlobalConfiguration&   global_configuration() { return global_configuration_; }
  configuration::Loader& config_loader() { return config_loader_; }
  shared_ptr<Model>      model() { return master_model_; }
  shared_ptr<ThreadPool> thread_pool() { return thread_pool_; }

private:
  // methods
  bool RunQuery();
  bool RunEstimation();
  int  RunMCMC();

  // members
  RunMode::Type             run_mode_ = RunMode::kInvalid;
  GlobalConfiguration       global_configuration_;
  utilities::RunParameters  run_parameters_;
  configuration::Loader     config_loader_;
  shared_ptr<Model>         master_model_;
  shared_ptr<ThreadPool>    thread_pool_;
  utilities::StandardHeader standard_header;
};

} /* namespace niwa */

#endif /* SOURCE_RUNNER_H_ */
