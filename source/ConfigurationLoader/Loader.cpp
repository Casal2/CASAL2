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
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>

#include "Categories/Categories.h"
#include "File.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "InitialisationPhases/Factory.h"
#include "Model/Model.h"
#include "Penalties/Factory.h"
#include "Processes/Factory.h"
#include "Reports/Factory.h"
#include "Selectivities/Factory.h"
#include "TimeSteps/Factory.h"
#include "Translations/Translations.h"
#include "Utilities/To.h"
#include "Utilities/Logging/Logging.h"

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
    LOG_ERROR("Failed to open the first configuration file: " << config_file << ". Does this file exist? Is it in the right path?");

  file->Parse();

  LOG_INFO("file_lines_.size() == " << file_lines_.size());
  if (file_lines_.size() == 0)
    LOG_ERROR("The configuration file " << config_file << " is empty. Please specify a valid configuration file");

  ParseFileLines();
}

/**
 * This method will add the parameter line to our vector
 * of FileLines for parsing.
 *
 * @param line The file line information to store
 */
void Loader::AddFileLine(FileLine line) {
  file_lines_.push_back(line);
}

/**
 * This method will go through our loaded FileLines vector
 * and build a new vector for each block that is defined.
 * Once a vector has been built containing block data it'll
 * be passed through to ParseBlock(vector<string>) for parsing.
 *
 * This method will also store the current sub-type variables.
 */
void Loader::ParseFileLines() {
  LOG_TRACE();

  vector<FileLine> block;

  for (unsigned i = 0; i < file_lines_.size(); ++i) {
    if (file_lines_[i].line_ == "")
      continue;

    // Check if we're entering a new block
    if (file_lines_[i].line_[0] == '@') {
      if (block.size() > 0)
        ParseBlock(block);

      block.clear();
    }

    block.push_back(file_lines_[i]);
  }

  ParseBlock(block);
}

/**
 * This method will parse a single block from our loaded
 * configuration data. A block definition starts with an
 * @block line.
 *
 * @param block Vector of block's line definitions
 */
void Loader::ParseBlock(vector<FileLine> &block) {
  LOG_TRACE();
  if (block.size() == 0)
    LOG_CODE_ERROR("block.size() == 0");


  for(FileLine file_line : block) {
    LOG_INFO("file_line: " << file_line.file_name_ << ":" << file_line.line_number_ << " = " << file_line.line_);
  }

  /**
   * Get the block type
   * e.g
   * @block <label>
   */
  vector<string> line_parts;
  string block_type = "";
  string block_label = "";

  boost::split(line_parts, block[0].line_, boost::is_any_of(" "));
  if (line_parts.size() == 0)
    LOG_ERROR("At line " << block[0].line_number_ << " of " << block[0].file_name_
        << ": Could not successfully split the line into an array. Line is incorrectly formatted");
  if (line_parts.size() > 2)
    LOG_ERROR("At line " << block[0].line_number_ << " of " << block[0].file_name_
        << ": The block's label cannot have a space or tab in it. Please use alphanumeric characters and underscores only");

  block_type = line_parts[0].substr(1); // Skip the first char '@'
  block_label = line_parts.size() == 2 ? line_parts[1] : "";

  /**
   * Look for the object type
   * e.g
   * @block <label>
   * type <object_type>
   */
  string object_type = "";

  for(FileLine file_line : block) {
    if (file_line.line_.length() >= 5 && file_line.line_.substr(0, 4) == PARAM_TYPE) {

      // Split the line into a vector
      boost::split(line_parts, file_line.line_, boost::is_any_of(" "));
      if (line_parts.size() == 0)
        LOG_ERROR("At line " << file_line.line_number_ << " of " << file_line.file_name_
            << ": Could not successfully split the line into an array. Line is incorrectly formatted");

      if (line_parts.size() != 2)
        LOG_ERROR("At line " << file_line.line_number_ << " of " << file_line.file_name_
            << ": No valid value was specified as the type");

      object_type = line_parts[1];
      continue;
    }
  }

  /**
   * Create Object
   */
  block_type  = utilities::ToLowercase(block_type);
  object_type = utilities::ToLowercase(object_type);

  ObjectPtr object = CreateObject(block_type, object_type);
  if (!object)
    LOG_ERROR("At line " << block[0].line_number_ << " of " << block[0].file_name_
        << ": Block type or object type is invalid.\n"
        << "Block Type: " << block_type << "\n"
        << "Object Type: " << object_type);

  if (block_label != "" && !object->parameters().Add(PARAM_LABEL, block_label, block[0].file_name_, block[0].line_number_))
    LOG_ERROR("At line " << block[0].line_number_ << " of " << block[0].file_name_
        << ": The block @" << block_type << " does not support having a label");

  // Store where this object was defined for use in printing errors later
  object->set_block_type(block_type);
  object->set_defined_file_name(block[0].file_name_);
  object->set_defined_line_number(block[0].line_number_);

  /**
   * Load the parameters into our new object
   */
  string current_line      = "";
  bool   loading_table     = false;
  bool   loading_columns   = false;
  string table_label       = "";
  vector<string>           table_columns;
  vector<vector<string> >  table_data;

  // Iterate the loaded file lines for this block
  for(FileLine file_line : block) {
    current_line = file_line.line_;

    if (current_line.length() == 0)
      continue;
    if (current_line[0] == '@')
      continue; // Skip @block definition

    // Split the line
    boost::split(line_parts, current_line, boost::is_any_of(" "));
    if (line_parts.size() == 0)
      LOG_ERROR("At line " << file_line.line_number_ << " of " << file_line.file_name_
          << ": Could not successfully split the line into an array. Line is incorrectly formatted");

    // Load the parameters
    string parameter_type = util::ToLowercase(line_parts[0]);
    vector<string> values(line_parts.begin() + 1, line_parts.end());

    if (!loading_table && parameter_type == PARAM_TABLE) {
      // Start loading a new table
      loading_table   = true;
      loading_columns = true;

      if (line_parts.size() != 2)
        LOG_ERROR("At line " << file_line.line_number_ << " of " << file_line.file_name_
            << ": table parameter requires a valid label. Please use alphanumeric characters and underscores only");

      table_label = util::ToLowercase(line_parts[1]);
      table_data.clear();

    } else if (loading_table && loading_columns) {
      // We're on the line after the table <label> definition where the columns will be
      table_columns.assign(line_parts.begin(), line_parts.end());
      loading_columns = false;

    } else if (loading_table && parameter_type != PARAM_END_TABLE) {
      // We're loading a standard row of data for the table
      if (line_parts.size() != table_columns.size())
        LOG_ERROR("At line " << file_line.line_number_ << " of " << file_line.file_name_
            << ": Table data does not contain the correct number of columns. Expected (" << table_columns.size() << ") : Actual (" << line_parts.size() << ")");

      table_data.push_back(line_parts);

    } else if (loading_table && parameter_type == PARAM_END_TABLE) {
      // We've found the end of our table.
      if (line_parts.size() != 1)
        LOG_ERROR("At line " << file_line.line_number_ << " of " << file_line.file_name_
            << ": end_table cannot contain any extra values");

      loading_table   = false;
      loading_columns = false;

      if (!object->parameters().AddTable(table_label, table_columns, table_data, file_line.file_name_, file_line.line_number_))
        LOG_ERROR("At line " << file_line.line_number_ << " of " << file_line.file_name_
            << ": Could not add table '" << table_label << "' to block. Table is not supported");

    }

    if (object->parameters().IsDefined(parameter_type)) {
      const Parameter& parameter = object->parameters().Get(parameter_type);
      LOG_ERROR("At line " << file_line.line_number_ << " of " << file_line.file_name_
          << ": Parameter '" << parameter_type << "' was already specified at line " << parameter.line_number() << " of " << parameter.file_name());
    }

    if (!object->parameters().Add(parameter_type, values, file_line.file_name_, file_line.line_number_))
      LOG_ERROR("At line " << file_line.line_number_ << " of " << file_line.file_name_
          << ": Could not add parameter '" << parameter_type << "' to block '" << block_type << "'. Parameter is not supported");
  }
}

/**
 * This method will create an object pointer and return it so it
 * can be populated with it's configuration parameter values.
 *
 *  @param block_type The @block type that we want to create
 *  @param objec_type The type <object_type> value. It's a sub-type of the block type
 *  @return A shared_ptr to the object we've created
 */
ObjectPtr Loader::CreateObject(const string &block_type, const string &object_type) {

  ObjectPtr object;

  LOG_INFO("CreateObject = block_type: " << block_type << "; object_type: " << object_type);
  if (block_type == PARAM_MODEL) {
    object = Model::Instance();

  } else if (block_type == PARAM_AGEING) {
    object = processes::Factory::Create(block_type, object_type);

  } else if (block_type == PARAM_CATEGORIES) {
    object = Categories::Instance();

  } else if (block_type == PARAM_INITIALIZATION_PHASE) {
    object = initialisationphases::Factory::Create();

  } else if (block_type == PARAM_MATURATION) {
    object = processes::Factory::Create(block_type, object_type);

  } else if (block_type == PARAM_MORTALITY) {
    object = processes::Factory::Create(block_type, object_type);

  } else if (block_type == PARAM_PENALTY) {
    object = penalties::Factory::Create();

  } else if (block_type == PARAM_PROCESS) {
    object = processes::Factory::Create(block_type, object_type);

  } else if (block_type == PARAM_RECRUITMENT) {
    object = processes::Factory::Create(block_type, object_type);

  } else if (block_type == PARAM_REPORT) {
    object = reports::Factory::Create(block_type, object_type);

  } else if (block_type == PARAM_SELECTIVITY) {
    object = selectivities::Factory::Create(block_type, object_type);

  } else if (block_type == PARAM_TIME_STEP) {
    object = timesteps::Factory::Create();
  }

  return object;
}

} /* namespace configuration */
} /* namespace isam */


















