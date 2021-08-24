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
#include "MPD/MPD.h"
#include "Model/Model.h"
#include "ThreadPool/ThreadPool.h"

// namespaces
namespace niwa {

// class declaration
class Runner {
  friend class BaseThread;  // Unit test base model
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

  int                      Go();
  int                      GoWithRunMode(RunMode::Type run_mode);
  void                     set_run_parameters(utilities::RunParameters& options) { run_parameters_ = options; }
  utilities::RunParameters run_parameters() const { return run_parameters_; }
  GlobalConfiguration&     global_configuration() { return global_configuration_; }
  configuration::Loader&   config_loader() { return config_loader_; }
  shared_ptr<Model>        model();
  shared_ptr<ThreadPool>   thread_pool();
  shared_ptr<MPD>          mpd();

private:
  // methods
  bool RunQuery();
  bool RunEstimation();
  int  RunMCMC();

  // members
  RunMode::Type            run_mode_ = RunMode::kInvalid;
  GlobalConfiguration      global_configuration_;
  utilities::RunParameters run_parameters_;
  configuration::Loader    config_loader_;
  shared_ptr<Model>        master_model_;
  shared_ptr<ThreadPool>   thread_pool_;
  shared_ptr<MPD>          mpd_;
};

} /* namespace niwa */

#endif /* SOURCE_RUNNER_H_ */
