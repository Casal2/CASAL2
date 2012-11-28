/**
 * @file Loader.cpp
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
#include "Loader.h"

#include <fstream>
#include <iostream>
#include <algorithm>

#include "File.h"
#include "Translations/Translations.h"
#include "Utilities/To.h"
#include "Utilities/Logging/Logging.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "Model/Model.h"

// Namespaces
using std::ifstream;
using std::cout;
using std::endl;

namespace util = isam::utilities;

namespace isam {
namespace configuration {

/**
 * Load the configuration file into our system. This method will
 * query the global_configuration for the name of the configuration
 * file to load.
 *
 * Once it has the name it'll load the file into a member vector
 * then start parsing it.
 */
void Loader::LoadConfigFile() {

  /**
   * Check if we want to skip loading the configuration files or not. This is utilised
   * by the unit testing suite to allow us to load values from memory
   */
  bool skip_loading_file = GlobalConfiguration::Instance()->skip_config_file();
  if (skip_loading_file)
    return;

  /**
   * Open our first configuration file and start loading it
   */
  string config_file = GlobalConfiguration::Instance()->config_file();
  FilePtr file = FilePtr(new File(this));
  if (!file->OpenFile(config_file))
    LOG_ERROR("Failed to open the primary configuration file: " << config_file);

  file->Parse();

  if (needs_new_object_)
    LOG_ERROR("The configuration files are invalid. Either the file has not specified any block objects "
        << "or it ends with a @block definition that has no valid parameters");
}

/**
 * Create a new internal object pointer to the isam::BaseObject we want to
 * assign the parameters too.
 */
bool Loader::CreateNewObject() {
  LOG_INFO("Creating new object. Defined by @" << block_type_ << " with type parameter '" << object_type_ << "'");

  if (block_type_ == PARAM_MODEL) {
    current_object_ = Model::Instance();


  } else
    return false;

  needs_new_object_ = false;
  return true;
}

} /* namespace configuration */
} /* namespace isam */


















