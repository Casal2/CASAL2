/**
 * @file GlobalConfiguration.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "GlobalConfiguration.h"

#include "Common/BaseClasses/Object.h"
#include "Common/Logging/Logging.h"
#include "Common/Model/Model.h"
#include "Common/Reports/Factory.h"
#include "Common/Utilities/To.h"

namespace niwa {
namespace util = niwa::utilities;

/**
 * This method will clear any previously set parameters within our global configuration.
 * All member objects need to be cleared during this method.
 */
void GlobalConfiguration::Clear() {
  options_ = utilities::RunParameters();
  command_line_parameters_.clear();
}

/**
 * This method will take any values that we've stored from
 * the command line and load them into the global configuration.
 *
 * This is done after we parse the configuration files because
 * some of the information in the configuration files will
 * be modified by the command line.
 *
 * We'll also parse some of the extra options here and create objects we may need to create
 */
void GlobalConfiguration::ParseOptions(Model* model) {
  LOG_TRACE();

  if (options_.override_random_number_seed_)
    options_.random_number_seed_ = options_.override_rng_seed_value_;

  if (options_.output_ != "") {
    auto report = reports::Factory::Create(model, PARAM_REPORT, PARAM_OUTPUT_PARAMETERS);
    report->parameters().Add(PARAM_LABEL, "parameter_output", __FILE__, __LINE__);
    report->parameters().Add(PARAM_TYPE, PARAM_OUTPUT_PARAMETERS, __FILE__, __LINE__);
    report->parameters().Add(PARAM_FILE_NAME, options_.output_, __FILE__, __LINE__);
    report->set_skip_tags(true);
  }
}

} /* namespace niwa */
