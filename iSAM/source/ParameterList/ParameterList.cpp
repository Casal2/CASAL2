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

#include "Translations/Translations.h"
#include "Utilities/Logging/Logging.h"
#include "Utilities/To.h"

// Using
namespace util = isam::utilities;
using std::map;
using std::vector;
using std::string;
using std::cout;
using std::endl;

namespace isam {

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
bool ParameterList::AddTable(const string& label, const vector<string>& columns, const vector<vector<string> >& data, const string& file_name, const unsigned& line_number) {
  TablePtr table = TablePtr(new isam::parameterlist::Table(label));
  table->AddColumns(columns);
  for (vector<string> row : data)
    table->AddRow(row);

  ParameterTable parameter_table;
  parameter_table.file_name_    = file_name;
  parameter_table.line_number_  = line_number;
  parameter_table.table_        = table;
  return true;
}

/**
 *
 */
void ParameterList::Populate() {
  auto iter = parameters_.begin();

  /**
   * go through and look for missing required parameters
   */
  string missing_parameters = "";
  for (; iter != parameters_.end(); ++iter) {
    if (iter->second->values().size() == 0 && !iter->second->is_optional())
      missing_parameters += iter->first + " ";
  }

  if (missing_parameters != "") {
    if (parameters_.find(PARAM_LABEL) == parameters_.end()) {
      LOG_ERROR("At line " << defined_line_number_ << " of file " << defined_file_name_ << " the following required parameters for the block "
          "are required but have not been defined: " << missing_parameters);

    } else {
      auto parameter = parameters_.find(PARAM_LABEL);
      LOG_ERROR(parameter->second->location() << " the following parameters are required but have not been defined: " << missing_parameters);
    }
  }

  for (iter = parameters_.begin(); iter != parameters_.end(); ++iter)
    iter->second->Bind();
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
 * This method will copy all of the parameters from
 * the source parameter list into this parameter list.
 *
 * NOTE: The TablesPtr are not recreated.
 *
 * @param source The source parameter list
 */
void ParameterList::CopyFrom(const ParameterList& source) {
  LOG_TRACE();
  this->defined_file_name_    = source.defined_file_name_;
  this->defined_line_number_  = source.defined_line_number_;
  this->parent_block_type_    = source.parent_block_type_;

  map<string, ParameterPtr>::const_iterator iter;
  for (iter = source.parameters_.begin(); iter != source.parameters_.end(); ++iter) {
    const vector<string> values = iter->second->values();
    for (string value : values) {
      LOG_INFO("Copying value: " << value << " from " << iter->first);
      parameters_[iter->first]->AddValue(value);
    }
  }

  map<string, ParameterTable>::const_iterator iter2;
  for (iter2 = source.tables_.begin(); iter2 != source.tables_.end(); ++iter2)
    tables_[iter2->first] = iter2->second;
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
  if (iter == parameters_.end()) {
    LOG_CODE_ERROR("parameters_ object is missing the parameter: " << label);
  }

  return iter->second->location() + ": " + label;
}

} /* namespace isam */
