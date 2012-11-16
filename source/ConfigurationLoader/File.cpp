/**
 * @file File.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "File.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>

#include "Loader.h"
#include "Utilities/Logging/Logging.h"
#include "Utilities/String.h"

// Namespaces
namespace isam {
namespace configuration {

namespace util = isam::utilities;

/**
 * Default Constructor
 */
File::File(Loader* loader)
  : loader_(loader) {
}

/**
 * Destructor
 */
File::~File() {
  if (file_.is_open())
    file_.close();
}

/**
 * Attempt to open our configuration file
 *
 * @param file_name The name of the file to open
 * @return true on success, false on failure
 */
bool File::OpenFile(string file_name) {
  LOG_TRACE();

  file_name_ = file_name;

  file_.open(file_name_.c_str());
  if (file_.fail() || !file_.is_open())
    return false;

  return true;
}

/**
 * Parse the configuration file. Creating objects and loading
 * the parameter objects
 */
void File::Parse() {
  LOG_TRACE();

  if (file_.fail() || !file_.is_open())
    LOG_ERROR("Unable to parse the configuration file because a previous error has not been reported.\nFile: " << file_name_);

  /**
   * Iterate through our file parsing the contents
   */
  bool      multi_line_comment  = false;
  string    current_line        = "";
  while (getline(file_, current_line)) {
    ++line_number_;

    if (current_line.length() == 0)
      continue;

    /**
     * If we're in a multi-line comment we need to look for the end, or discard the line
     */
    if (multi_line_comment) {
      size_t pos = current_line.find_first_of(CONFIG_MULTI_COMMENT_END);
      if (pos == string::npos)
        continue;

      multi_line_comment = false;
      current_line = current_line.substr(pos + 1, current_line.length() - pos);
    }

    /**
     * Check if we're entering a multi-line comment. Strip any comment parts of the line
     */
    size_t pos = current_line.find_first_of(CONFIG_MULTI_COMMENT_START);
    if (pos != string::npos) {
      multi_line_comment = true;
      current_line = current_line.substr(0, pos);
    }

    /**
     * Check and remove any single-line (end of line) comments
     * e.g <data> #comment
     */
    pos = current_line.find_first_of(CONFIG_SINGLE_COMMENT);
    if (pos != string::npos)
      current_line = current_line.substr(0, pos);

    if (current_line.length() == 0)
      continue;

    /**
     * Change tabs to spaces, remove any leading/trailing or multiple spaces
     * so we can be sure the input is nicely formatted
     */
    boost::replace_all(current_line, "\t", " ");
    boost::trim_all(current_line);

    /**
     * Now we need to check if this line is an include line for a new
     * file.
     */
    if (current_line.substr(0, strlen(CONFIG_INCLUDE)) == CONFIG_INCLUDE) {
      LOG_INFO("current_line: " << current_line);
      size_t first_quote = current_line.find_first_of("\"");
      size_t last_quote  = current_line.find_last_of("\"");

      if (first_quote == string::npos || last_quote == string::npos || first_quote == last_quote)
        LOG_ERROR("Unable to find a matching pair of quotes surrounding the file name for command " << CONFIG_INCLUDE << "\n"
            << "File: " << file_name_ << " (Line: " << line_number_ << ")");

      string include_name = current_line.substr(first_quote);
      boost::replace_all(include_name, "\"", "");
      LOG_INFO("Loading new configuration file via include " << include_name);

      FilePtr include_file = FilePtr(new File(loader_));
      if (!include_file->OpenFile(include_name))
        LOG_ERROR("Unable to open the file specified as an include '" << include_name << "'\n"
            << "File: " << file_name_ << " (Line: " << line_number_ << ")");

      include_file->Parse();
      continue;
    }

    /**
     * At this point everything is standard. We have a simple line of text that we now need to parse. All
     * comments etc have been removed and we've gone through any include_file directives
     */
    ParseLine(current_line);
  } // while(get_line())
}

/**
 * Parse a single line from our configuration file.
 *
 * @param line The line to parse
 */
void File::ParseLine(string &line) {
  LOG_TRACE();

  // Split our line into a vector based on space
  vector<string> line_parts;
  boost::split(line_parts, line, boost::is_any_of(" "));
  if (line_parts.size() == 0)
    LOG_ERROR("Could not successfully split the line into a vector"
        << "Data: " << line << "\n"
        << "File: " << file_name_ << " (Line: " << line_number_ << ")");

  // Check if we're entering a new block type
  if (line_parts[0][0] == '@') {
    loader_->set_block_type(line_parts[0].substr(1));
    loader_->set_object_type("");

    string block_label = line_parts.size() >= 2 ? line_parts[1] : "";
    loader_->set_block_label(block_label);
    return;
  }

  // Check if we have an object type or not.
  if (line.substr(0, 4) == PARAM_TYPE) {
    loader_->set_object_type(line.substr(5));
    LOG_INFO("Object type has been found: " << loader_->object_type());
    return;
  }

  // Check if we need to create a new object now
  if (loader_->needs_new_object()) {
    if (!(loader_->CreateNewObject()))
      LOG_ERROR("Failed to create a new object from configuration file block\n"
          << "Data: @" << loader_->block_type() << " | type: " << loader_->object_type() << "\n"
          << "File: " << file_name_ << " (Line: " << line_number_-1 << ")");

    string block_label = loader_->block_label();
    if (block_label != "" && !loader_->current_object()->parameters().Add(PARAM_LABEL, block_label))
      LOG_ERROR("Failed to define label for new block. Label's are not supported for this block type\n"
          << "Data: @" << loader_->block_type() << " | type: " << loader_->object_type() << "\n"
          << "File: " << file_name_ << " (Line: " << line_number_-1 << ")");
  }

  ObjectPtr object = loader_->current_object();

  /*
   * Now check if we're currently loading data for a table or not
   */
  bool loading_table = loader_->loading_table();
  if (!loading_table && line_parts[0] == PARAM_TABLE) {
    loader_->set_loading_table(true);
    if (line_parts.size() != 2)
      LOG_ERROR("No valid label has been given for the table in the configuration file.\n"
          << "File: " << file_name_ << " (Line: " << line_number_ << ")");

    // Add new table and tell the loader about it
    loader_->set_current_table(object->parameters().AddTable(line_parts[1]));
    return;
  }

  // Now check for the end of a table block
  if (loading_table && line_parts[0] == PARAM_END_TABLE) {
    loader_->set_loading_table(false);
    return;
  }

  /**
   * At this point we're in 1 of 2 states.
   * 1. We have a normal variable with N values to store
   * 2. We are processing a table and we have either data or columns to store
   */
  if (!loading_table) {
    vector<string> values;
    values.assign(line_parts.begin() + 1, line_parts.end());

    if (object->parameters().HasParameter(line_parts[0]))
        LOG_ERROR("Parameter '" << line_parts[0] << "' has already been defined for section @" << loader_->block_type() << "\n"
            << "Line: " << line << "\n"
            << "File: " << file_name_ << " (Line: " << line_number_ << ")");

    if (!object->parameters().Add(line_parts[0], values))
      LOG_ERROR("Failed to add parameter to section @" << loader_->block_type() << " because it's not valid\n"
          << "Line: " << line << "\n"
          << "File: " << file_name_ << " (Line: " << line_number_ << ")");

  } else {
    TablePtr table = loader_->current_table();
    if (table->HasColumns()) {
      table->AddRow(line_parts);
    } else {

      unsigned column_count = table->ColumnCount();
      if (column_count != line_parts.size())
        LOG_ERROR("The data does not contain the same number of columns as the definition for the table\n"
            << "Expected: " << column_count << "\n"
            << "Actual: " << line_parts.size() << "\n"
            << "File: " << file_name_ << " (Line: " << line_number_ << ")");

      table->AddColumns(line_parts);
    }
  }
}

} /* namespace configuration */
} /* namespace isam */



















