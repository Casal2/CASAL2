/**
 * @file InternalEmptyModel.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 20/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

#include "InternalEmptyModel.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>

#include "Age/AgeLengths/Manager.h"
#include "Common/Catchabilities/Manager.h"
#include "Common/Categories/Categories.h"
#include "Common/DerivedQuantities/Manager.h"
#include "Common/Estimates/Manager.h"
#include "Common/GlobalConfiguration/GlobalConfiguration.h"
#include "Common/InitialisationPhases/Manager.h"
#include "Common/LengthWeights/Manager.h"
#include "Common/Minimisers/Manager.h"
#include "Common/Model/Model.h"
#include "Common/ObjectiveFunction/ObjectiveFunction.h"
#include "Common/Observations/Manager.h"
#include "Common/Partition/Accessors/Category.h"
#include "Common/Partition/Partition.h"
#include "Common/Penalties/Manager.h"
#include "Common/Processes/Manager.h"
#include "Common/Reports/Manager.h"
#include "Common/Selectivities/Manager.h"
#include "Common/TimeSteps/Manager.h"
#include "Common/Logging/Logging.h"
#include "Common/Utilities/RandomNumberGenerator.h"
#include "Common/Utilities/To.h"

namespace niwa {
namespace testfixtures {
/**
 *
 */
void InternalEmptyModel::SetUp() {
  Base::SetUp();

  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  rng.Reset(2468);

  configuration_file_.clear();
  model_->global_configuration().flag_skip_config_file();
}

/**
 * Add a new line to our internal configuration vector so we can load it.
 *
 * @param line The contents of the line
 * @param file_name The name of the file where the line has been defined
 * @param line_number The line number where the line has been defined
 */
void InternalEmptyModel::AddConfigurationLine(const string& line, const string& file_name, unsigned line_number) {
  vector<string> lines;
  boost::split(lines, line, boost::is_any_of("\n"));

  unsigned i = 0;
  for (string split_line : lines) {
    config::FileLine new_line;
    new_line.line_ = split_line;
    new_line.file_name_ = file_name;
    new_line.line_number_ = line_number + i;

    configuration_file_.push_back(new_line);

    i++;
  }
}

/**
 * Call our configuration file loader to load our internal
 * configuration file for execution in the model
 */
void InternalEmptyModel::LoadConfiguration() {
  configuration::Loader loader(*model_);

  for (config::FileLine file_line : configuration_file_)
    loader.AddFileLine(file_line);

  loader.LoadConfigFile();
  loader.ParseFileLines();
}

} /* namespace sizeweights */
} /* namespace niwa */
#endif /* TESTMODE */
