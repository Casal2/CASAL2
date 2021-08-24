/**
 * @file Base.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2/09/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "BaseThreaded.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include "../../AgeLengths/Manager.h"
#include "../../Catchabilities/Manager.h"
#include "../../Categories/Categories.h"
#include "../../DerivedQuantities/Manager.h"
#include "../../Estimates/Manager.h"
#include "../../GlobalConfiguration/GlobalConfiguration.h"
#include "../../InitialisationPhases/Manager.h"
#include "../../LengthWeights/Manager.h"
#include "../../Logging/Logging.h"
#include "../../Minimisers/Manager.h"
#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Model/Models/Age.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../Observations/Manager.h"
#include "../../Partition/Accessors/Category.h"
#include "../../Partition/Partition.h"
#include "../../Penalties/Manager.h"
#include "../../Processes/Manager.h"
#include "../../Reports/Manager.h"
#include "../../Selectivities/Manager.h"
#include "../../TimeSteps/Manager.h"
#include "../../Utilities/RandomNumberGenerator.h"
#include "../../Utilities/To.h"

// namespaces
namespace niwa {
namespace testfixtures {

/**
 *
 */
void BaseThreaded::SetUp() {
  configuration_file_.clear();

  runner_.reset(new niwa::Runner());

  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  rng.Reset(666);

  utilities::RunParameters run_parameters;
  run_parameters.override_rng_seed_value_ = 2468;
  run_parameters.no_std_report_           = true;
  run_parameters.run_mode_                = RunMode::kTesting;
  run_parameters.create_mpd_output_file_  = false;
  run_parameters.estimate_before_mcmc_    = true;

  runner_->set_run_parameters(run_parameters);
  runner_->global_configuration().set_skip_config_file(true);
}

/**
 * Tear down the test fixture
 */
void BaseThreaded::TearDown() {
  // Stop any threads that have been spawned.
  // We do this hear instead of in the runner to make
  // sure it's done cleanly
  if (runner_ != nullptr && runner_->thread_pool())
    runner_->thread_pool()->TerminateAll();
}

/**
 * Add a new line to our internal configuration vector so we can load it.
 *
 * @param line The contents of the line
 * @param file_name The name of the file where the line has been defined
 * @param line_number The line number where the line has been defined
 */
void BaseThreaded::AddConfigurationLine(const string& line, const string& file_name, unsigned line_number) {
  vector<string> lines;
  boost::split(lines, line, boost::is_any_of("\n"));

  unsigned i = 0;
  for (string split_line : lines) {
    config::FileLine new_line;
    new_line.line_        = split_line;
    new_line.file_name_   = file_name;
    new_line.line_number_ = line_number + i;

    configuration_file_.push_back(new_line);

    i++;
  }
}

/**
 * Call our configuration file loader to load our internal
 * configuration file for execution in the model
 */
void BaseThreaded::LoadConfiguration() {
  auto& loader = runner_->config_loader();
  for (config::FileLine file_line : configuration_file_) {
    loader.StoreLine(file_line);
  }
}

} /* namespace testfixtures */
} /* namespace niwa */
#endif /* TESTMODE */
