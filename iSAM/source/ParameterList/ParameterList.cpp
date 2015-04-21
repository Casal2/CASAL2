/**
 * @file CParameterList.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/02/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2011 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "ParameterList.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include "Categories/Categories.h"
#include "Translations/Translations.h"
#include "Logging/Logging.h"
#include "Utilities/String.h"
#include "Utilities/To.h"

// Using
namespace util = niwa::utilities;
using std::map;
using std::vector;
using std::string;
using std::cout;
using std::endl;

namespace niwa {

/**
 * Add a single value to our parameter list
 *
 * @param label The label for the parameter
 * @param value The value of the parameter
 * @param file_name The file where the parameter was specified
 * @param line_number The line number where the parameter was specified
 * @return true on success, false on failure
 */
bool ParameterList::Add(const string& label, const vector<string>& values, const string& file_name, const unsigned& line_number) {
  if (parameters_.find(label) == parameters_.end())
    return false;

  auto iter = parameters_.find(label);
  iter->second->set_values(values);
  iter->second->set_file_name(file_name);
  iter->second->set_line_number(line_number);

  return true;
}

/**
 * Add a single value to our parameter list
 *
 * @param label The label for the parameter
 * @param value The value of the parameter
 * @param file_name The file where the parameter was specified
 * @param line_number The line number where the parameter was specified
 * @return true on success, false on failure
 */
bool ParameterList::Add(const string& label, const string& value, const string& file_name, const unsigned& line_number) {
  vector<string> new_values;
  new_values.push_back(value);

  return this->Add(label, new_values, file_name, line_number);
}

/**
 * Add a new table to our parameter list
 *
 * @param label The label for the table
 * @param columns A vector containing the columns
 * @param data A double vector containing the data
 * @param file_name Name of file where table definition finished
 * @param line_number Line number where table definition finished
 * @return true on success, false on failure
 */
//bool ParameterList::AddTable(const string& label, const vector<string>& columns, const vector<vector<string> >& data, const string& file_name, const unsigned& line_number) {
//  if (tables_.find(label) == tables_.end())
//    return false;
//
//  parameters::TablePtr table = tables_.find(label)->second;
//  if (table->requires_comlums())
//    table->AddColumns(columns);
//  else
//    table->AddRow(columns);
//  for (vector<string> row : data)
//    table->AddRow(row);
//  table->set_file_name(file_name);
//  table->set_line_number(line_number);
//
//  return true;
//}

/**
 *
 */
void ParameterList::Populate() {
  /**
   * go through and look for missing required parameters
   */
  string missing_parameters = "";
  for (auto iter = parameters_.begin(); iter != parameters_.end(); ++iter) {
    if (iter->second->values().size() == 0 && !iter->second->is_optional())
      missing_parameters += iter->first + " ";
  }
  for (auto iter = tables_.begin(); iter != tables_.end(); ++iter) {
    if (iter->second->data().size() == 0 && !iter->second->is_optional())
      missing_parameters += iter->first + "(Table) ";
  }

  if (missing_parameters != "") {
    if (parameters_.find(PARAM_LABEL) == parameters_.end()) {
      LOG_ERROR() << "At line " << defined_line_number_ << " in " << defined_file_name_ << " the following required parameters for the block "
                  << parent_block_type_ << " are required but have not been defined: " << missing_parameters;
    } else {
      auto parameter = parameters_.find(PARAM_LABEL);
      if (parameter->second->values().size() == 0) {
        LOG_ERROR() << "At line " << defined_line_number_ << " in " << defined_file_name_ << " the following required parameters for the block "
            << parent_block_type_ << " are required but have not been defined: " << missing_parameters;
      } else {
        LOG_ERROR() << parameter->second->location() << " the following parameters are required but have not been defined: " << missing_parameters;
      }
    }
  }

  // handle categories
  if (parameters_.find(PARAM_CATEGORIES) != parameters_.end() || parameters_.find(PARAM_CATEGORY) != parameters_.end()) {
    ParameterPtr parameter;
    if (parameters_.find(PARAM_CATEGORIES) != parameters_.end())
      parameter = parameters_[PARAM_CATEGORIES];
    else
      parameter = parameters_[PARAM_CATEGORY];
    vector<string> new_values = Categories::Instance()->ExpandLabels(parameter->values(), parameter);
    parameter->set_values(new_values);
  }

  // NOTE: This has to be last
  // bind parameters
  LOG_FINEST() << "Binding parameters for @" << parent_block_type_ << " defined at line " << defined_line_number_ << " in " << defined_file_name_;
  for (auto iter = parameters_.begin(); iter != parameters_.end(); ++iter) {
    if (iter->second->values().size() == 0 && !iter->second->is_optional())
      continue;
    iter->second->Bind();
  }

  if (parameters_.find(PARAM_LABEL) != parameters_.end()) {
    ParameterPtr param = parameters_[PARAM_LABEL];
    if (param->values().size() != 0) {
      string invalid = utilities::String::find_invalid_characters(param->values()[0]);
      if (invalid != "")
        LOG_ERROR() << param->location() << " the label '" << param->values()[0] << "' contains the following invalid characters: " << invalid;
    }
  }
}

/**
 * Return a constant reference to one of our parameter objects.
 *
 * NOTE: This method MUST be called with a valid label otherwise
 * a reference to an empty parameter will be returned.
 *
 * @param label The parameter to return
 * @return The parameter reference
 */
const ParameterPtr ParameterList::Get(const string& label) {
  map<string, ParameterPtr>::iterator iter = parameters_.find(label);
  if (iter == parameters_.end())
    return ParameterPtr();

  return iter->second;
}

/**
 * Return a constant pointer to one of our parameter tables
 *
 * @param label of the table to return
 * @return
 */
const parameters::TablePtr ParameterList::GetTable(const string& label) {
  auto iter = tables_.find(label);
  if (iter == tables_.end())
    return parameters::TablePtr();

  return iter->second;
}

/**
 * This method will copy all of the parameters from
 * the source parameter list into this parameter list.
 *
 * NOTE: The TablesPtr are not recreated.
 *
 * @param source The source parameter list
 * @param parameter_label The parameter to copy over
 */
void ParameterList::CopyFrom(const ParameterList& source, string parameter_label) {
  LOG_TRACE();
  this->defined_file_name_    = source.defined_file_name_;
  this->defined_line_number_  = source.defined_line_number_;
  this->parent_block_type_    = source.parent_block_type_;

  auto iter = source.parameters_.find(parameter_label);
  if (iter == source.parameters_.end()) {
    LOG_CODE_ERROR() << "iter == source.parameters_.end() for label: " << parameter_label;
  }

  Add(parameter_label, iter->second->values(), iter->second->file_name(), iter->second->line_number());
}

/**
 *
 */
void ParameterList::CopyFrom(const ParameterList& source, string parameter_label, const unsigned &value_index) {
  LOG_TRACE();
  auto iter = source.parameters_.find(parameter_label);
  if (iter == source.parameters_.end())
    LOG_CODE_ERROR() << "iter == source.parameters_.end() for label: " << parameter_label;
  if (iter->second->values().size() == 0)
    return;

  vector<string> values;
  if (iter->second->values().size() <= value_index)
    LOG_CODE_ERROR() << "iter->second->values().size(" << iter->second->values().size() << ") <= value_index(" << value_index << "): " << parameter_label;

  values.push_back(iter->second->values()[value_index]);
  Add(parameter_label, values, iter->second->file_name(), iter->second->line_number());
}

/**
 *
 */
void ParameterList::Clear() {
  auto iter = parameters_.begin();
  for (; iter != parameters_.end(); ++iter) {
    iter->second->Clear();
  }

  tables_.clear();
}


/**
 * Find the location string for one of our parameters.
 *
 * @param label The label for the parameter
 * @return The location string for an error message
 */
string ParameterList::location(const string& label) {
  map<string, ParameterPtr>::iterator iter = parameters_.find(label);
  auto table_iter = tables_.find(label);
  if (iter == parameters_.end() && table_iter == tables_.end()) {
    LOG_CODE_ERROR() << "parameters_ object is missing the parameter: " << label;
  }

  if (iter != parameters_.end())
    return iter->second->location() + " the parameter '" + label + "' ";

  return table_iter->second->location() + " the table '"  + label + "' ";
}

/**
 * Bind a table pointer to our map so it can be recognised and retrieved by the configuration loader
 *
 * @param label The label of the table to bind
 * @param table The pointer to the table we're binding
 * @param description used for documentation, ignored
 * @param values used for documentation, ignored
 */
void ParameterList::BindTable(const string& label, parameters::TablePtr table, const string& description, const string& values, bool requires_columns, bool optional) {
  table->set_requires_columns(requires_columns);
  table->set_is_optional(optional);
  tables_[label] = table;
}


} /* namespace niwa */
