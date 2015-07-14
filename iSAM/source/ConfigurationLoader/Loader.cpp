/**
 * @file Loader.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 */

// Headers
#include "Loader.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>

#include "Factory/Object.h"
#include "File.h"
#include "GlobalConfiguration/GlobalConfiguration.h"
#include "Translations/Translations.h"
#include "Utilities/String.h"
#include "Utilities/To.h"
#include "Logging/Logging.h"

// Namespaces
namespace niwa {
namespace configuration {

using std::ifstream;
using std::cout;
using std::endl;

namespace util = niwa::utilities;

/**
 * Load the configuration file into our system. This method will
 * query the global_configuration for the name of the configuration
 * file to load.
 *
 * Once it has the name it'll load the file into a member vector
 * then start parsing it.
 */
void Loader::LoadConfigFile(const string& override_file_name) {

  /**
   * Check if we want to skip loading the configuration files or not. This is utilised
   * by the unit testing suite to allow us to load values from memory
   */
  bool skip_loading_file = GlobalConfiguration::Instance()->skip_config_file();
  if (!skip_loading_file) {
    /**
     * Open our first configuration file and start loading it
     */
    string config_file = GlobalConfiguration::Instance()->config_file();
    if (override_file_name != "")
      config_file = override_file_name;

    FilePtr file = FilePtr(new File(this));
    if (!file->OpenFile(config_file))
      LOG_FATAL() << "Failed to open the first configuration file: " << config_file << ". Does this file exist? Is it in the right path?";

    file->Parse();

    LOG_FINEST() << "file_lines_.size() == " << file_lines_.size();
    if (file_lines_.size() == 0)
      LOG_FATAL() << "The configuration file " << config_file << " is empty. Please specify a valid configuration file";
  }

  ParseFileLines();
}

/**
 * This method will add the parameter line to our vector
 * of FileLines for parsing.
 *
 * @param line The file line information to store
 */
void Loader::AddFileLine(FileLine line) {
  boost::replace_all(line.line_, "\t", " ");
  boost::replace_all(line.line_, "  ", " ");
  boost::trim_all(line.line_);

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

    if (file_lines_[i].line_.find(";") != string::npos) {
      bool inside_inline_declaration = false;
      string line = "";
      for (char c : file_lines_[i].line_) {
        if (c == '[')
          inside_inline_declaration = true;
        if (c == ']')
          inside_inline_declaration = false;

        if (c == ';' && !inside_inline_declaration) {
          FileLine new_line;
          new_line.file_name_     = file_lines_[i].file_name_;
          new_line.line_number_   = file_lines_[i].line_number_;
          new_line.line_          = line;

          block.push_back(new_line);
          line = "";

        } else
          line += c;
      }

      FileLine new_line;
      new_line.file_name_     = file_lines_[i].file_name_;
      new_line.line_number_   = file_lines_[i].line_number_;
      new_line.line_          = line;
      block.push_back(new_line);

    } else
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
    LOG_CODE_ERROR() << "block.size() == 0";

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
    LOG_FATAL() << "At line " << block[0].line_number_ << " in " << block[0].file_name_
        << ": Could not successfully split the line into an array. Line is incorrectly formatted";
  if (line_parts.size() > 2)
    LOG_FATAL() << "At line " << block[0].line_number_ << " in " << block[0].file_name_
        << ": The block's label cannot have a space or tab in it. Please use alphanumeric characters and underscores only";

  block_type = line_parts[0].substr(1); // Skip the first char '@'
  block_label = line_parts.size() == 2 ? line_parts[1] : "";

  /**
   * Look for the object type
   * e.g
   * @block <label>
   * type <object_type>
   */
  string sub_type = "";

  for(FileLine file_line : block) {
    if (file_line.line_.length() >= 5 && file_line.line_.substr(0, 4) == PARAM_TYPE) {

      // Split the line into a vector
      boost::split(line_parts, file_line.line_, boost::is_any_of(" "));
      if (line_parts.size() == 0)
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
            << ": Could not successfully split the line into an array. Line is incorrectly formatted";

      if (line_parts.size() != 2)
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
            << ": No valid value was specified as the type";

      sub_type = line_parts[1];
      continue;
    }
  }

  /**
   * Create Object
   */
  block_type  = utilities::ToLowercase(block_type);
  sub_type = utilities::ToLowercase(sub_type);

  ObjectPtr object = factory::Object::Create(block_type, sub_type);
  if (!object)
    LOG_FATAL() << "At line " << block[0].line_number_ << " in " << block[0].file_name_
        << ": Block object type or sub-type is invalid.\n"
        << "Object Type: " << block_type << "\n"
        << "Sub-Type: " << sub_type;

  if (block_label != "" && !object->parameters().Add(PARAM_LABEL, block_label, block[0].file_name_, block[0].line_number_))
    LOG_FATAL() << "At line " << block[0].line_number_ << " in " << block[0].file_name_
        << ": The block @" << block_type << " does not support having a label";

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

  // Iterate the loaded file lines for this block
  for(FileLine file_line : block) {
    if (file_line.line_.length() == 0)
      continue;
    if (file_line.line_[0] == '@')
      continue; // Skip @block definition

    string parent_label = block_label == "" ? block_type : block_label;
    HandleInlineDefinitions(file_line, parent_label);

    current_line = file_line.line_;

    // Split the line
    boost::split(line_parts, current_line, boost::is_any_of(" "));
    if (line_parts.size() == 0)
      LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
          << ": Could not successfully split the line into an array. Line is incorrectly formatted";

    // Load the parameters
    string parameter_type = util::ToLowercase(line_parts[0]);
    vector<string> values(line_parts.begin() + 1, line_parts.end());

    /**
     * Re-process the values based on the symbols we support
     */
    string error = "";
    if (parameter_type != PARAM_PARAMETER && !HandleOperators(values, error))
      LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
          << ": " << error;

    /**
     *
     */
    if (!loading_table && parameter_type == PARAM_TABLE) {
      // Start loading a new table
      loading_table   = true;
      loading_columns = true;

      if (line_parts.size() != 2)
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
            << ": table parameter requires a valid label. Please use alphanumeric characters and underscores only";

      table_label = util::ToLowercase(line_parts[1]);
      current_table_ = object->parameters().GetTable(table_label);
      if (!current_table_)
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
            << ": table " << table_label << " is not a supported table label.";
      current_table_->set_file_name(file_line.file_name_);
      current_table_->set_line_number(file_line.line_number_);

    } else if (loading_table && loading_columns) {
      // We're on the line after the table <label> definition where the columns will be
      if (!current_table_)
        LOG_CODE_ERROR() << "!current_table";

      if (current_table_->requires_comlums())
        current_table_->AddColumns(line_parts);
      else
        current_table_->AddRow(line_parts);
      loading_columns = false;

    } else if (loading_table && parameter_type != PARAM_END_TABLE) {
      // We're loading a standard row of data for the table
      if (current_table_->requires_comlums() && line_parts.size() != current_table_->GetColumnCount())
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
            << ": Table data does not contain the correct number of columns. Expected (" << current_table_->GetColumnCount() << ") : Actual (" << line_parts.size() << ")";

      current_table_->AddRow(line_parts);

    } else if (loading_table && parameter_type == PARAM_END_TABLE) {
      // We've found the end of our table.
      if (line_parts.size() != 1)
        LOG_ERROR() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
            << ": end_table cannot contain any extra values";

      loading_table   = false;
      loading_columns = false;
      current_table_ = parameters::TablePtr();

    } else {
      // loading a normal parameter
      const ParameterPtr parameter = object->parameters().Get(parameter_type);
      if (!parameter) {
        LOG_ERROR() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
            << ": Parameter '" << parameter_type << "' is not supported";
      } else if (parameter->has_been_defined()) {
        LOG_ERROR() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
            << ": Parameter '" << parameter_type << "' for object " << block_type
            << " was already specified at line " << parameter->line_number() << " in " << parameter->file_name();
      } else if (!object->parameters().Add(parameter_type, values, file_line.file_name_, file_line.line_number_)) {
        LOG_ERROR() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
            << ": Could not add parameter '" << parameter_type << "' to block '" << block_type << "'. Parameter is not supported";
      }
    }
  }
}

/**
 * This method will search through the line for values that have operators in them and then
 * rebuild the vector with the values properly split out as they should be.
 *
 * Operators:
 * + = Join 2 values together as a single value
 * , = These values are 1 parameter (e.g 1,2,4,5
 * - = Split integer range values out (e.g 1-3 = 1,2,3
 *
 * @param line_values The vector containing the split parts we want to modify
 * @return true on success, false on failure
 */
bool Loader::HandleOperators(vector<string>& line_values, string &error) {
  string line = boost::algorithm::join(line_values, " ");
  line_values.clear();

  LOG_FINEST() << "HandleOperators: " << line;

  /**
   * Join operator + is used to join multiple categories together
   * so a process/observation can work on them as one.
   *
   * We don't actually need to do anything about this as removing
   * the spaces is sufficient
   */
  boost::replace_all(line, " +", "+");
  boost::replace_all(line, "+ ", "+");

  /**
   * The list operator is used when defining categories and
   * must be combined with the . (category format) to make
   * larger entries
   */
  boost::replace_all(line, " .", ".");
  boost::replace_all(line, ". ", ".");

  boost::replace_all(line, " ,", ",");
  boost::replace_all(line, ", ", ",");

  /**
   * The range operator is used to define numerical ranges
   * either in ascending or descending configuration
   */
  boost::replace_all(line, " :", ":");
  boost::replace_all(line, ": ", ":");

  /**
   * The multiply operator is used to define something once
   * then have multiple copies of it in the configuration file
   */
  boost::replace_all(line, " *", "*");
  boost::replace_all(line, "* ", "*");

  /**
   * Branch the line back in to values now that we've removed
   * any weird spaces or tabs.
   */
  vector<string> temp_line_values;
  boost::split(temp_line_values, line, boost::is_any_of(" "));

  for (string line_value : temp_line_values) {
    // the chunks is a 2D vector we're going to build then
    // put back together into the output
    vector<vector<string>> chunks;

    // break each line value in to segments (with . character)
    vector<string> line_value_segments;
    boost::split(line_value_segments, line_value, boost::is_any_of("."));

    for (unsigned i = 0; i < line_value_segments.size(); ++i) {
      chunks.push_back(vector<string>());

      vector<string> segment_chunks;
      boost::split(segment_chunks, line_value_segments[i], boost::is_any_of(","));
      for (string chunk : segment_chunks) {
        if (chunk.find(":") != string::npos) {
          /**
           * Handle the : range operator. We split the chunk
           * in to 2 pieces and iterate between the two filling
           * in the range
           */
          vector<string> numerics;
          boost::split(numerics, chunk, boost::is_any_of(":"));
          if (numerics.size() != 2) {
            error = "Could not split line with : properly";
            return false;
          }

          int start_value;
          int end_value;
          if (util::To<int>(numerics[0], start_value) && util::To<int>(numerics[1], end_value)) {
            if (start_value < end_value) {
              for (int value = start_value; value <= end_value; ++value)
                chunks[i].push_back(util::ToInline<int, string>(value));
            } else {
              for (int value = start_value; value >= end_value; --value)
                chunks[i].push_back(util::ToInline<int, string>(value));
            }
          }  else
            chunks[i].push_back(chunk);

        } else if (chunk.find("*") != string::npos && chunk != "*" && chunk != "*+") {
          /**
           * Handle the * multiplier operator. We split it on the *
           * character and then multiple the first segment by the
           * amount specified in the second segment
           */
          vector<string> temp;
          boost::split(temp, chunk, boost::is_any_of("*"));
          if (temp.size() != 2) {
            error = "Could not split line with * multiplier";
            return false;
          }

          unsigned multiplier = 0;
          if (!util::To<unsigned>(temp[1], multiplier)) {
            error = "Could not convert " + temp[1] + " to an unsigned int";
            return false;
          }

          for (unsigned index = 0; index < multiplier; ++index)
            chunks[i].push_back(temp[0]);
        } else
          chunks[i].push_back(chunk);
          // default is to do nothing
      }
    }

    /**
     * Build our outputs up
     */
    vector<string> combinations = (*chunks.rbegin());
    for (auto iter = chunks.rbegin() + 1; iter != chunks.rend(); ++iter) {
      vector<string> temp = combinations;
      combinations.clear();
      for (unsigned j = 0; j < (*iter).size(); ++j) {
        for (string value : temp) {
          value = (*iter)[j] + "." + value;
          combinations.push_back(value);
        }
      }
    }

    for (string combination : combinations)
      line_values.push_back(combination);
  }

  return true;
}

/**
 * This method will check the line for inline definitions and
 * if any are found it'll extract them, create the defined
 * object and replace the definition with a label
 * to be used by the parent object
 *
 * All labels created will be prefixed with <parent>.
 */
void Loader::HandleInlineDefinitions(FileLine& file_line, const string& parent_label) {

  /**
   * Check if this line contains an inline definition we need to process first
   */
  size_t first_inline_bracket  = file_line.line_.find("[");
  size_t second_inline_bracket = file_line.line_.find("]");

  if (first_inline_bracket != string::npos && second_inline_bracket != string::npos) {
    unsigned inline_count = 0;
    vector<std::pair<string, string> > replacement_strings;
    string full_definition = "";

    /**
     * Loop through all inline definition blocks
     */
    while (first_inline_bracket != string::npos) {
      if (first_inline_bracket >= second_inline_bracket)
        LOG_CODE_ERROR() << "first_inline_bracket (" << first_inline_bracket << ") <= second_inline_bracket (" << second_inline_bracket << ")";
      if (first_inline_bracket <= 1)
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
            << ": You cannot start the line with an inline definition [ operator";

      /**
       * Work out the block type for use when defining it
       */
      size_t space_loc = file_line.line_.find(' ');
      if (space_loc == string::npos)
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
            << ": This line contains no space characters so we cannot determine the label for the inline definition";

      string block_type = file_line.line_.substr(0, space_loc);
      // do some quick changes to handle weird parameter names
      LOG_FINEST() << "block_type: " << block_type;
      if (block_type.find(PARAM_SELECTIVITIES) != string::npos)
        block_type = PARAM_SELECTIVITY;
      LOG_FINEST() << "block_type: " << block_type;

      /**
       * Get the label. Either it's going to be the first part of the line
       * e.g <label> x y z
       * or it's going to be part of the inline definition
       * e.g timestep label=[definition]
       */
      string label = "";

      // This means we've got a label defined
      if (file_line.line_[first_inline_bracket - 1] == '=') {
        string start_string = file_line.line_.substr(0, first_inline_bracket - 1);
        size_t space_loc = start_string.find_last_of(' ');
        if (space_loc == string::npos)
          LOG_CODE_ERROR() << "space_loc == string::npos for line: " << start_string;

        label = parent_label + string(".") + start_string.substr(space_loc + 1);
        full_definition = file_line.line_.substr(space_loc + 1, second_inline_bracket - space_loc);

      } else {
        label = parent_label + string(".") + utilities::ToInline<unsigned, string>(++inline_count);
        full_definition = file_line.line_.substr(first_inline_bracket, second_inline_bracket - first_inline_bracket + 1);
      }

      LOG_FINEST() << "Inline definition label: " << label << " | full definition: " << full_definition;
      if (full_definition.find('=') != string::npos) {
        replacement_strings.push_back(std::pair<string, string>(full_definition, label));

        /**
         * Now we have to split the string up between the definition block
         */
        string definition = file_line.line_.substr(first_inline_bracket+1, second_inline_bracket - first_inline_bracket - 1);
        LOG_FINEST() << "Absolute definition: " << definition;

        vector<string> definition_parts;
        boost::split(definition_parts, definition, boost::is_any_of(";"));

        vector<FileLine> inline_block;
        FileLine block_line;
        block_line.file_name_    = file_line.file_name_;
        block_line.line_number_  = file_line.line_number_;
        block_line.line_         = "@" + block_type + " " + label;
        inline_block.push_back(block_line);

        for(string& definition : definition_parts) {
          boost::replace_all(definition, "=", " ");
          boost::trim_all(definition);

          FileLine new_line;
          new_line.file_name_     = file_line.file_name_;
          new_line.line_number_   = file_line.line_number_;
          new_line.line_          = definition;
          inline_block.push_back(new_line);
        }

        ParseBlock(inline_block);
      }
      LOG_FINEST() << "first_inline_bracket: " << first_inline_bracket << "; second_inline_bracket: " << second_inline_bracket;

      first_inline_bracket  = file_line.line_.find("[", second_inline_bracket);
      second_inline_bracket = file_line.line_.find("]", second_inline_bracket+1);

      LOG_FINEST() << "first_inline_bracket: " << first_inline_bracket << "; second_inline_bracket: " << second_inline_bracket;
      // Check to ensure rest of the line has been defined properly.
      if ( (first_inline_bracket != string::npos && second_inline_bracket == string::npos) ||
           (first_inline_bracket == string::npos && second_inline_bracket != string::npos)) {
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
            << ": This line contains either a [ or a ] but not both. This line is not in a valid format";
      }
    } // while (first_inline_bracket != string::npos)

    /**
     * Now we have to replace the original line with the inline definitions
     * with the labels for the new objects we've created.
     */
    for(std::pair<string, string> replacement : replacement_strings)
      boost::replace_first(file_line.line_, replacement.first, replacement.second);
    LOG_FINEST() << "Finished line replacement for inline definitions. New line: " << file_line.line_;

  } else if (first_inline_bracket != string::npos || second_inline_bracket != string::npos) {
    // Only one brace was present.
    LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
        << ": This line contains either a [ or a ] but not both. This line is not in a valid format";
  }

  return; // no braces so no inline
}

} /* namespace configuration */
} /* namespace niwa */


















