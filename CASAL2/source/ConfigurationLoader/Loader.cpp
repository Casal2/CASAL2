/**
 * @file Loader.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 */

// Headers
#include "Loader.h"

#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <fstream>
#include <iostream>
#include <regex>

#include "../ConfigurationLoader/File.h"
#include "../Logging/Logging.h"
#include "../Model/Factory.h"
#include "../Translations/Translations.h"
#include "../Utilities/String.h"
#include "../Utilities/To.h"

// Namespaces
namespace niwa {
namespace configuration {

using std::cout;
using std::endl;
using std::ifstream;

namespace util = niwa::utilities;

/**
 * Load the configuration files from disk to memory.
 */
bool Loader::LoadFromDiskToMemory(GlobalConfiguration& global_config, const string& override_file_name) {
  /**
   * Check if we want to skip loading the configuration files or not. This is utilised
   * by the unit testing suite to allow us to load values from memory
   */
  bool skip_loading_file = global_config.skip_loading_config_file();
  if (!skip_loading_file) {
    /**
     * Open our first configuration file and start loading it
     */
    string config_file = global_config.config_file();
    if (override_file_name != "")
      config_file = override_file_name;

    File file(*this);
    if (!file.OpenFile(config_file))
      LOG_FATAL() << "Failed to open the first configuration file: " << config_file << ". Does this file exist? Is it in the right path?";

    file.LoadIntoMemory();

    LOG_FINE() << "file_lines_.size() == " << file_lines_.size();
    if (file_lines_.size() == 0)
      LOG_FATAL() << "The configuration file " << config_file << " is empty. Please specify a valid configuration file";
  } else {
    LOG_FINEST() << "Skipping the load file for configuration file: " << global_config.config_file();
  }

  // Create block objects in memory and find minimiser type
  LOG_FINEST() << "Creating blocks from input";
  CreateBlocksFromInput();
  LOG_FINEST() << "Handling inline declarations";
  HandleInlineDefinitions();
  LOG_FINEST() << "Finding active minimiser type";
  FindActiveMinimiserType();

  LOG_FINEST() << "Finised loading configuration from disk to memory";
  return true;
}

/**
 * This method will add the parameter line to the vector
 * of FileLines for parsing.
 *
 * @param line The file line information to store
 */
void Loader::StoreLine(FileLine line) {
  boost::replace_all(line.line_, "\t", " ");
  boost::replace_all(line.line_, "  ", " ");
  boost::trim_all(line.line_);

  file_lines_.push_back(line);
}

/**
 * This method will go through the loaded FileLines vector
 * and build a new vector for each block that is defined.
 *
 * A block is a collection of parameters for the same object.
 * e.g.
 * @model
 * start_year 1
 * final_year 2
 *
 * ^^  This would be a single block. There is a 1:1 relationship
 * between blocks and objects created via the model.factory()
 *
 * Note: This is public because we want to be able to
 *
 */
void Loader::CreateBlocksFromInput() {
  LOG_TRACE();
  blocks_.clear();
  vector<FileLine> block;
  bool             first_block = true;
  for (unsigned i = 0; i < file_lines_.size(); ++i) {
    if (file_lines_[i].line_ == "")
      continue;

    // Check if we're entering a new block
    if (file_lines_[i].line_[0] == '@') {
      if (block.size() > 0) {
        if (first_block) {
          if (utilities::ToLowercase(block[0].line_) != "@model")
            LOG_FATAL() << "The first block to be processed must be @model. Actual was " << block[0].line_;

          bool found_model_type = false;
          for (auto& file_line : block) {
            if (file_line.line_.length() > 4 && utilities::ToLowercase(file_line.line_.substr(0, 4)) == PARAM_TYPE) {
              found_model_type = true;

              vector<string> line_parts;
              boost::split(line_parts, file_line.line_, boost::is_any_of(" "));
              if (line_parts.size() != 2)
                LOG_FATAL() << "@model type must be either age, length or pi_approx";
              model_type_ = line_parts[1];
            }
          }

          if (!found_model_type) {
            LOG_FINEST() << "@model.type is not specified. Using the default " << PARAM_AGE;
            model_type_ = PARAM_AGE;
          }
        }  // if (first_block)

        blocks_.push_back(block);
        first_block = false;
      }
      block.clear();
    }

    block.push_back(file_lines_[i]);
  }

  blocks_.push_back(block);
}

/**
 * This method will parse a single block from the loaded
 */
void Loader::Build(vector<shared_ptr<Model>>& model_list) {
  for (auto model : model_list) {
    for (auto& block : blocks_) {
      ParseBlock(model, block);
    }
  }
}

/**
 * This method will parse a single block from our loaded
 * configuration data. A block definition starts with an
 * @block line.
 *
 * @param block Vector of block's line definitions
 */
void Loader::ParseBlock(shared_ptr<Model> model, vector<FileLine>& block) {
  LOG_TRACE();
  if (block.size() == 0)
    LOG_CODE_ERROR() << "block.size() == 0";

  /**
   * Get the block type
   * e.g
   * @block <label>
   */
  vector<string> line_parts;
  string         block_type  = "";
  string         block_label = "";
  LOG_FINEST() << "Block[0].line_: " << block[0].line_;
  boost::split(line_parts, block[0].line_, boost::is_any_of(" "));
  if (line_parts.size() == 0)
    LOG_FATAL() << "At line " << block[0].line_number_ << " in " << block[0].file_name_ << ": Could not successfully split the line into an array. Line is incorrectly formatted";
  if (line_parts.size() > 2)
    LOG_FATAL() << "At line " << block[0].line_number_ << " in " << block[0].file_name_
                << ": The block's label cannot have a space or tab in it. Please use alphanumeric characters and underscores only";

  block_type  = line_parts[0].substr(1);  // Skip the first char '@'
  block_label = line_parts.size() == 2 ? line_parts[1] : "";

#ifdef USE_AUTODIFF
  // We're using auto-diff. So we want to skip loading MCMCs
  if (block_type == PARAM_MCMC)
    return;
#endif

  /**
   * Look for the object type and partition_type
   * e.g
   * @block <label>
   * type <object_type>
   */
  string        sub_type       = "";
  PartitionType partition_type = PartitionType::kModel;

  for (FileLine file_line : block) {
    string& line = file_line.line_;

    if (line.length() >= 5 && line.substr(0, 4) == PARAM_TYPE) {
      // Split the line into a vector
      boost::split(line_parts, file_line.line_, boost::is_any_of(" "));
      if (line_parts.size() == 0)
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
                    << ": Could not successfully split the line into an array. Line is incorrectly formatted";

      if (line_parts.size() != 2)
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_ << ": No valid value was specified as the type";

      sub_type = line_parts[1];
      continue;

    } else if (line.length() >= 19 && line.substr(0, 19) == PARAM_PARTITION_TYPE) {
      // Split the line into a vector
      boost::split(line_parts, file_line.line_, boost::is_any_of(" "));
      if (line_parts.size() == 0)
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
                    << ": Could not successfully split the line into an array. Line is incorrectly formatted";

      if (line_parts.size() != 2)
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_ << ": No valid value was specified as the partition_type";

      string temp = utilities::ToLowercase(line_parts[1]);
      if (!utilities::To<PartitionType>(temp, partition_type))
        LOG_FATAL() << "Could not convert value " << temp << " to a valid partition_type (model, age, length, hybrid)";
      if (partition_type == PartitionType::kModel)
        partition_type = model->partition_type();

      continue;
    }
  }  // for(FileLine file_line : block)

  /**
   * Create Object
   */
  block_type = utilities::ToLowercase(block_type);
  sub_type   = utilities::ToLowercase(sub_type);

  // We don't create reports on threads, only primary
  if (block_type == PARAM_REPORT && !model->is_primary_thread_model()) {
    LOG_MEDIUM() << "skipping report." << sub_type << " because current model is not the primary thread model";
    return;
  }
  // We don't create Minimisers on threads, only the primary model
  if (block_type == PARAM_MINIMIZER && !model->is_primary_thread_model()) {
    LOG_MEDIUM() << "skipping minimiser." << sub_type << " because current model is not the primary thread model";
    return;
  }
  // We don't create MCMC on threads, only the primary model
  if (block_type == PARAM_MCMC && !model->is_primary_thread_model()) {
    LOG_MEDIUM() << "skipping MCMC." << sub_type << " because current model is not the primary thread model";
    return;
  }

  Object* object = model->factory().CreateObject(block_type, sub_type, partition_type);
  if (!object)
    LOG_FATAL() << "At line " << block[0].line_number_ << " in " << block[0].file_name_ << ": Block object type or sub-type is invalid.\n"
                << "Object Type: " << block_type << "\n"
                << "Sub-Type: " << sub_type;

  if (block_label != "" && !object->parameters().Add(PARAM_LABEL, block_label, block[0].file_name_, block[0].line_number_))
    LOG_FATAL() << "At line " << block[0].line_number_ << " in " << block[0].file_name_ << ": The block @" << block_type << " does not support having a label";

  // Store where this object was defined for use in printing errors later
  object->set_block_type(block_type);
  object->set_defined_file_name(block[0].file_name_);
  object->set_defined_line_number(block[0].line_number_);

  /**
   * If this is the model, tell it what type of model it is
   */
  //	if (block_type == PARAM_MODEL && sub_type != "") {
  //		if (!utilities::To<PartitionType>(sub_type, partition_type))
  //			LOG_FATAL()
  //			<< "Could not convert value " << sub_type << " to a valid type (age, length, hybrid) for the @model block";
  //		model->set_partition_type(partition_type);
  //	}

  /**
   * Load the parameters into our new object
   */
  string current_line    = "";
  bool   loading_table   = false;
  bool   loading_columns = false;
  string table_label     = "";

  // Iterate the loaded file lines for this block
  for (FileLine file_line : block) {
    if (file_line.line_.length() == 0)
      continue;
    if (file_line.line_[0] == '@')
      continue;  // Skip @block definition

    current_line = file_line.line_;

    // Split the line
    boost::split(line_parts, current_line, boost::is_any_of(" "));
    if (line_parts.size() == 0)
      LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
                  << ": Could not successfully split the line into an array. Line is incorrectly formatted";

    // Load the parameters
    string parameter_type = util::ToLowercase(line_parts[0]);

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

      table_label    = util::ToLowercase(line_parts[1]);
      current_table_ = object->parameters().GetTable(table_label);
      if (!current_table_)
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_ << ": table " << table_label << " is not a supported table label.";
      current_table_->set_file_name(file_line.file_name_);
      current_table_->set_line_number(file_line.line_number_);

    } else if (loading_table && loading_columns) {
      /**
       * Loading column headers from the table if they exist.
       */
      vector<string> values(line_parts.begin(), line_parts.end());
      if (!current_table_)
        LOG_CODE_ERROR() << "!current_table";

      if (current_table_->requires_columns())
        current_table_->AddColumns(values);
      else
        current_table_->AddRow(values);
      loading_columns = false;

    } else if (loading_table && parameter_type != PARAM_END_TABLE) {
      /**
       * Inside a Table
       */
      vector<string> values(line_parts.begin(), line_parts.end());

      // We're loading a standard row of data for the table
      if (current_table_->requires_columns() && values.size() != current_table_->column_count()) {
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_ << ": Table data does not contain the correct number of columns. Expected ("
                    << current_table_->column_count() << ") : Actual (" << values.size() << ")\n"
                    << boost::join(values, ", ");
      }

      current_table_->AddRow(values);

    } else if (loading_table && parameter_type == PARAM_END_TABLE) {
      // We've found the end of our table.
      if (line_parts.size() != 1)
        LOG_ERROR() << "At line " << file_line.line_number_ << " in " << file_line.file_name_ << ": end_table cannot contain any extra values";

      loading_table   = false;
      loading_columns = false;
      current_table_  = nullptr;

    } else {
      /**
       * Normal Parameter
       */
      vector<string> values(line_parts.begin() + 1, line_parts.end());
      string         error = "";
      if (parameter_type != PARAM_PARAMETER && parameter_type != PARAM_EQUATION && !util::String::HandleOperators(values, error))
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_ << ": " << error;

      // loading a normal parameter
      if (!object->parameters().ignore_all_parameters()) {
        const Parameter* parameter = object->parameters().Get(parameter_type);
        if (!parameter) {
          LOG_ERROR() << "At line " << file_line.line_number_ << " in " << file_line.file_name_ << ": Parameter '" << parameter_type << "' is not supported";
        } else if (parameter->has_been_defined()) {
          LOG_ERROR() << "At line " << file_line.line_number_ << " in " << file_line.file_name_ << ": Parameter '" << parameter_type << "' for object " << block_type
                      << " was already specified at line " << parameter->line_number() << " in " << parameter->file_name();
        } else if (!object->parameters().Add(parameter_type, values, file_line.file_name_, file_line.line_number_)) {
          LOG_ERROR() << "At line " << file_line.line_number_ << " in " << file_line.file_name_ << ": Could not add parameter '" << parameter_type << "' to block '" << block_type
                      << "'. Parameter is not supported";
        }

        string line = boost::algorithm::join(values, " ");
        LOG_FINEST() << "Storing values: [" << parameter_type << "]: " << line;
      } else {
        string line = boost::algorithm::join(values, " ");
        LOG_FINEST() << "Ignoring values: [" << parameter_type << "]: " << line;
      }
    }  // if (!loading_table && parameter_type == PARAM_TABLE)
  }    // for(FileLine file_line : block)

  if (block_type == "model") {
    model->PopulateParameters();
  }
}

/**
 * This method will check the line for inline definitions and
 * if any are found it'll extract them, create the defined
 * object and replace the definition with a label
 * to be used by the parent object
 *
 * All labels created will be prefixed with <parent>.
 *
 * Inline declarations can be either:
 * label=[declaration] OR
 * [declaration]
 *
 * e.g.
 * time_steps step_one=[type=iterative; processes=recruitment ageing]
 * time_steps [type=iterative; processes=recruitment ageing]
 *
 * Note: Every inline declaration is required to have a ; character
 * at some point because at a minumum it needs to define type + one other parameter
 *
 */
void Loader::HandleInlineDefinitions() {
  vector<string>           replacements = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
  vector<string>           temp;  // temp to old outputs of splits we only want for a short period
  vector<vector<FileLine>> new_blocks;

  for (auto& block : blocks_) {
    string parent_label = "";

    // Do a quick check to see if we have any inline declarations in this block
    bool has_inline = false;
    for (auto& file_line : block) {
      std::regex e(".*[\\[].*=.*[\\]].*");
      if (std::regex_match(file_line.line_, e)) {
        LOG_FINEST() << "Found inline declaration on line " << file_line.line_;
        has_inline = true;
        break;
      }
    }
    if (!has_inline)
      continue;

    // From here on, we know we have an inline declaration in this block
    LOG_FINEST() << "Inline declaration found";
    for (auto& file_line : block) {
      if (file_line.line_.length() == 0)
        LOG_CODE_ERROR() << "if (file_line.line_.length() == 0)";

      // Try to find parent name
      LOG_FINEST() << "line: " << file_line.line_;
      if (file_line.line_[0] == '@') {
        if (file_line.line_ == "@model") {
          parent_label = "model";
        } else {
          boost::split(temp, file_line.line_, boost::is_any_of(" "));
          if (temp.size() == 1) {
            LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_ << ": The @block must have a label because it has an inline declaration";
          } else
            parent_label = temp[1];  // handles @block label
        }
        continue;
      }
      LOG_FINEST() << "Parent label: " << parent_label;

      // see if this line contains an inline
      if (file_line.line_.find(";") == string::npos && file_line.line_.find("=") == string::npos)
        continue;  // next line please
      if (file_line.line_.length() > 9 && file_line.line_.substr(0, 9) == PARAM_PARAMETER)
        continue;  // no inline declarations on parameter line

      /**
       * Check if this line contains an inline definition we need to process first
       */
      size_t first_inline_bracket  = file_line.line_.find("[");
      size_t second_inline_bracket = file_line.line_.find("]");

      if (first_inline_bracket != string::npos && second_inline_bracket != string::npos) {
        LOG_FINEST() << "Found first pair of [ and ] braces";
        vector<std::pair<string, string>> replacement_strings;
        string                            full_definition = "";

        /**
         * Loop through all inline definition blocks
         */
        while (first_inline_bracket != string::npos) {
          if (first_inline_bracket >= second_inline_bracket)
            LOG_CODE_ERROR() << "first_inline_bracket (" << first_inline_bracket << ") <= second_inline_bracket (" << second_inline_bracket << ")";
          if (first_inline_bracket <= 1)
            LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_ << ": Do not start the line with an inline definition '[' operator";

          /**
           * Work out the block type for use when defining it
           */
          size_t space_loc = file_line.line_.find(' ');
          if (space_loc == string::npos)
            LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
                        << ": This line contains no space characters so the label for the inline definition cannot be determined";

          string block_type = file_line.line_.substr(0, space_loc);
          LOG_FINEST() << "block_type: " << block_type;
          if (block_type == PARAM_SELECTIVITIES || block_type == PARAM_LOSS_RATE_SELECTIVITIES)
            block_type = PARAM_SELECTIVITY;
          LOG_FINEST() << "confirmed block_type: " << block_type;

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
            size_t space_loc    = start_string.find_last_of(' ');
            if (space_loc == string::npos)
              LOG_CODE_ERROR() << "space_loc == string::npos for line: " << start_string;

            label           = parent_label + string(".") + start_string.substr(space_loc + 1);
            full_definition = file_line.line_.substr(space_loc + 1, second_inline_bracket - space_loc);

          } else {
            unsigned index   = inline_count_[block_type + "." + parent_label];
            string   s_index = index < replacements.size() ? replacements[index] : utilities::ToInline<unsigned, string>(++index);
            ++inline_count_[block_type + "." + parent_label];

            label           = parent_label + string(".") + s_index;
            full_definition = file_line.line_.substr(first_inline_bracket, second_inline_bracket - first_inline_bracket + 1);
          }

          LOG_FINEST() << "Inline definition label: " << label << " | full definition: " << full_definition;
          if (full_definition.find('=') != string::npos) {
            replacement_strings.push_back(std::pair<string, string>(full_definition, label));

            /**
             * Now we have to split the string up between the definition block
             */
            string definition = file_line.line_.substr(first_inline_bracket + 1, second_inline_bracket - first_inline_bracket - 1);
            LOG_FINEST() << "Absolute definition: " << definition;

            vector<string> definition_parts;
            boost::split(definition_parts, definition, boost::is_any_of(";"));

            vector<FileLine> inline_block;
            FileLine         block_line;
            block_line.file_name_   = file_line.file_name_;
            block_line.line_number_ = file_line.line_number_;
            block_line.line_        = "@" + block_type + " " + label;
            inline_block.push_back(block_line);

            for (string& definition : definition_parts) {
              boost::replace_all(definition, "=", " ");
              boost::trim_all(definition);

              FileLine new_line;
              new_line.file_name_   = file_line.file_name_;
              new_line.line_number_ = file_line.line_number_;
              new_line.line_        = definition;
              inline_block.push_back(new_line);
            }

            new_blocks.push_back(inline_block);
          }
          LOG_FINEST() << "first_inline_bracket: " << first_inline_bracket << "; second_inline_bracket: " << second_inline_bracket;

          first_inline_bracket  = file_line.line_.find("[", second_inline_bracket);
          second_inline_bracket = file_line.line_.find("]", second_inline_bracket + 1);

          LOG_FINEST() << "first_inline_bracket: " << first_inline_bracket << "; second_inline_bracket: " << second_inline_bracket;
          // Check to ensure rest of the line has been defined properly.
          if ((first_inline_bracket != string::npos && second_inline_bracket == string::npos) || (first_inline_bracket == string::npos && second_inline_bracket != string::npos)) {
            LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
                        << ": This line contains either a '[' or a ']' but not both. This line is not in a valid format";
          }
        }  // while (first_inline_bracket != string::npos)

        /**
         * Now we have to replace the original line with the inline definitions
         * with the labels for the new objects we've created.
         */
        for (std::pair<string, string> replacement : replacement_strings) boost::replace_first(file_line.line_, replacement.first, replacement.second);
        LOG_FINEST() << "Finished line replacement for inline definitions. New line: " << file_line.line_;

      } else if (first_inline_bracket != string::npos || second_inline_bracket != string::npos) {
        // Only one brace was present.
        LOG_FATAL() << "At line " << file_line.line_number_ << " in " << file_line.file_name_
                    << ": This line contains either a '[' or a ']' but not both. This line is not in a valid format";
      }
    }  // for (auto& file_line : file_lines_) {
  }    // for (auto& block : blocks_) {

  // add new inline blocks to the object
  for (auto block : new_blocks) blocks_.push_back(block);

  return;  // no braces so no inline
}

/**
 * @brief This method will parse through the Block data looking for the
 * minimiser definitions and storing the type so we can use this
 * to determine what type of Model objects we should be creating.
 *
 */
void Loader::FindActiveMinimiserType() {
  LOG_TRACE();

  minimiser_type_ = "";  // reset member
  vector<string> split_string;

  // Hold information about each minimiser
  typedef struct {
    string name_   = "<<unknown>>";
    string type_   = "";
    bool   active_ = true;
  } MinimiserDefinition;
  vector<MinimiserDefinition> minimisers;

  for (auto block_vector : blocks_) {
    if (block_vector[0].line_.substr(1, 9) != PARAM_MINIMIZER)
      continue;  // skip non @minimiser

    LOG_FINEST() << "Found @minimiser definiton @ line " << block_vector[0].line_number_ << " of file " << block_vector[0].file_name_;
    // This is working through definition of a single @minimiser block
    MinimiserDefinition new_definition;
    for (auto file_line : block_vector) {
      // Try and extract minimiser name if one exists
      if (file_line.line_.substr(1, 9) == PARAM_MINIMIZER) {
        LOG_FINEST() << "Minimiser name block: " << file_line.line_;
        boost::split(split_string, file_line.line_, boost::is_any_of(" "));
        if (split_string.size() == 2)
          new_definition.name_ = split_string[1];

        continue;
      }
      // Try and extra type of minimiser
      if (file_line.line_.substr(0, 4) == PARAM_TYPE) {
        LOG_FINEST() << "Minimiser type block: " << file_line.line_;
        boost::split(split_string, file_line.line_, boost::is_any_of(" "));
        if (split_string.size() == 2)
          new_definition.type_ = split_string[1];

        continue;
      }

      // Try and extra if minimiser is active or not
      if (file_line.line_.substr(0, 6) == PARAM_ACTIVE) {
        boost::split(split_string, file_line.line_, boost::is_any_of(" "));
        if (split_string.size() == 2) {
          if (!utilities::To<bool>(split_string[1], new_definition.active_)) {
            LOG_FATAL() << "Failed to determine if value " << split_string[1] << " is a boolean type (true/false) "
                        << " in file " << file_line.file_name_ << " at line " << file_line.line_;
          }
        }
      }
    }  // for (auto file_line : block_vector)

    // add new definition to lise
    if (new_definition.active_)
      minimisers.push_back(new_definition);

  }  // for (auto block_vector : blocks_)

  /**
   * @brief Iterate over the defined minimisers
   * and store the type of the minimiser that is
   * going to be used during any estimations
   *
   * Note: We only care about first one, error handling is done elsewhere
   */
  if (minimisers.size() == 1) {
    minimiser_type_ = minimisers[0].type_;
  } else {
    for (auto minimiser : minimisers) {
      if (minimiser.active_) {
        minimiser_type_ = minimiser.type_;
        break;
      }
    }
  }
}

} /* namespace configuration */
} /* namespace niwa */
