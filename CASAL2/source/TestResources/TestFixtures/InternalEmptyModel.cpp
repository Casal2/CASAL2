/**
 * @file InternalEmptyModel.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 20/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

#include "InternalEmptyModel.h"

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
  model_->flag_primary_thread_model();
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
void InternalEmptyModel::LoadConfiguration() {
  configuration::Loader loader;

  for (config::FileLine file_line : configuration_file_) loader.StoreLine(file_line);

  loader.LoadFromDiskToMemory(model_->global_configuration());
  vector<shared_ptr<Model>> model_list;
  model_list.push_back(model_);
  loader.Build(model_list);
}

}  // namespace testfixtures
} /* namespace niwa */
#endif /* TESTMODE */
