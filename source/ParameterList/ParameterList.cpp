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
 * This method will add the name of an allowed parameter to the vector
 * for validation later using the .IsValid() method.
 *
 * @param label The name of the parameter we're allowing
 */
void ParameterList::RegisterAllowed(const string label) {
  LOG_INFO("Registering allowed parameter " << label);
  allowed_parameters_.push_back(label);
}

/**
 * Check if the parameter has been defined in our parameter list already
 * or not.
 *
 * @param label The label to check
 */
bool ParameterList::HasParameter(const string& label) {
  return parameters_.find(label) != parameters_.end();
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
bool ParameterList::Add(const string& label, const vector<string>& values, const string& file_name, const unsigned& line_number) {
  if (std::find(allowed_parameters_.begin(), allowed_parameters_.end(), label) == allowed_parameters_.end())
    return false;

  Parameter newParameter;
  newParameter.values_      = values;
  newParameter.file_name_   = file_name;
  newParameter.line_number_ = line_number;

  parameters_[label] = newParameter;

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
  if (std::find(allowed_parameters_.begin(), allowed_parameters_.end(), label) == allowed_parameters_.end())
    return false;

  Parameter newParameter;
  newParameter.values_.push_back(value);
  newParameter.file_name_   = file_name;
  newParameter.line_number_ = line_number;

  parameters_[label] = newParameter;

  return true;
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
  if (std::find(allowed_parameters_.begin(), allowed_parameters_.end(), label) == allowed_parameters_.end())
    return false;

  TablePtr table = TablePtr(new isam::parameter::Table(label));
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
 * Return a constant reference to one of our parameter objects.
 *
 * NOTE: This method MUST be called with a valid label otherwise
 * a reference to an empty parameter will be returned.
 *
 * @param label The parameter to return
 * @return The parameter reference
 */
const Parameter& ParameterList::Get(const string& label) {
  return parameters_[label];
}



































//
//
//
//
//
//
//
///**
// * Add a parameter to our parameter list. This is a standard
// * parameter with 1-n values, not a table.
// *
// * @param name The name of the parameter
// * @param value The value to add to the list of values for the parameter
// */
//void ParameterList::addParameter(string name, string value) {
//  parameters_[name].push_back(value);
//}
//
///**
// * Set the value of our parameter removing all other values beforehand.
// *
// * @param name The name of the value to clear and set
// * @param value The new value for this parameter
// */
//void ParameterList::setParameter(string name, string value) {
//  parameters_[name].clear();
//  parameters_[name].push_back(value);
//}
//
///**
// * This method adds a new complete table to our parameter list.
// *
// * @param name The name of the table to add
// * @param columns A vector containing the names of the columns for our table
// * @param data A vector containing the data for the table
// */
//void ParameterList::addTable(string name, vector<string> &columns, vector<string> &data) {
//
//  ParameterTable table;
//  table.columns_.assign(columns.begin(), columns.end());
//  table.data_.assign(data.begin(), data.end());
//
//  tables_[name].push_back(table);
//}
//
//void ParameterList::replaceTable(string tableName, vector<string> &columns, vector<string> &data) {
//
//  ParameterTable table;
//  table.columns_.assign(columns.begin(), columns.end());
//  table.data_.assign(data.begin(), data.end());
//
////  LOG_INFO("Replacing table: " + tableName);
//
//  string oldColumnCount = util::To<string>(tables_[tableName][0].columns_.size());
//  string oldDataCount   = util::To<string>(tables_[tableName][0].data_.size());
//
////  LOG_INFO("Column comparison: " + boost::lexical_cast<string>(columns.size()) + ":" + oldColumnCount);
////  LOG_INFO("Data comparison: " + boost::lexical_cast<string>(data.size()) + ":" + oldDataCount);
//
//  tables_[tableName].clear();
//  tables_[tableName].push_back(table);
//}
//
///**
// * Check if this object has the name'd parameter defined.
// *
// * @param name The name of the parameter to check
// * @param checkTables Do we check if we have a table with this name (default = true);
// * @return True if we do have a parameter, false otherwise
// */
//bool ParameterList::hasParameter(string name, bool checkTables) {
//
//  bool paramResult = parameters_.find(name) != parameters_.end();
//  bool tableResult = checkTables == true ? tables_.find(name) != tables_.end() : false;
//
//  return paramResult || tableResult;
//}
//
///**
// * Count how many tables we have with the parameter name.
// *
// * @param name The name of the table to count instances of
// */
//unsigned ParameterList::countTables(string name) {
//
//  if (tables_.find(name) == tables_.end())
//    return 0;
//
//  return tables_[name].size();
//}
//
///**
// * This method will iterate through all of the parameters/tables that have
// * been assigned to this object and check for a corresponding value
// * stored in the vAllowedParameters vector.
// *
// * If a parameter's label/name is not defined in vAllowedParameters
// * then it will throw an exception. To store an allowed value
// * you have to use the .registerAllowed() method.
// */
//void ParameterList::checkInvalidParameters(bool skipTables) {
//
//  // Check our standard parameters
//  map<string, vector<string> >::iterator mvPtr;
//
//  for (mvPtr = parameters_.begin(); mvPtr != parameters_.end(); mvPtr++) {
//    if (std::find(valid_parameters_.begin(), valid_parameters_.end(), (*mvPtr).first) == valid_parameters_.end())
//      THROW_EXCEPTION("Parameter not supported: " + (*mvPtr).first);
//  }
//
//  if (!skipTables) {
//    // Check our tables
//    map<string, vector<ParameterTable> >::iterator mvPtr2;
//    for (mvPtr2 = tables_.begin(); mvPtr2 != tables_.end(); mvPtr2++) {
//      if (std::find(valid_parameters_.begin(), valid_parameters_.end(), (*mvPtr2).first) == valid_parameters_.end())
//        THROW_EXCEPTION("Table not supported: " + (*mvPtr2).first);
//    }
//  }
//}
//
///**
// * This method will return a vector with all of the table names in it.
// */
//vector<string> ParameterList::getTableNames() {
//  vector<string> result;
//
//  map<string, vector<ParameterTable> >::iterator mvPtr;
//  for (mvPtr = tables_.begin(); mvPtr != tables_.end(); ++mvPtr) {
//    result.push_back(mvPtr->first);
//  }
//
//  return result;
//}
//
///**
// * This method will return the string value assigned to the parameter
// * <name>. If the value isn't there or if it has more than one value
// * assigned to it an exception will be thrown.
// *
// * @param name The name of the parameter to get the value for
// * @param optional Is this value an optional parameter (default = false)
// * @param defaultValue The default value to return if this value is missing (default = "")
// * @return The value as a string
// */
//string ParameterList::getString(string name, bool optional, string defaultValue) {
//  if (!hasParameter(name)) {
//    if (optional) {
//      return defaultValue;
//    } else {
//      THROW_EXCEPTION("Could not find parameter: " + name);
//    }
//  }
//
//  if ((int)parameters_[name].size() == 0)
//    THROW_EXCEPTION("Parameter has no values stored against it: " + name);
//
//  if ((int)parameters_[name].size() > 1) {
//    cout << "Too many values stored against parameter: " << name << endl;
//    cout << "Expected 1 value, but we had " << parameters_[name].size() << endl;
//    cout << "Values: ";
//    for (unsigned i = 0; i < parameters_[name].size(); ++i) {
//      cout << "'" << parameters_[name][i] << "' ";
//    }
//    cout << endl;
//
//    THROW_EXCEPTION("Could not return value for parameter as there is more than 1 value stored: " + name);
//  }
//
//  return parameters_[name][0];
//}
//
///**
// * This method will return the double value assigned to the parameter
// * <name>. If the value isn't there or if it has more than one value
// * assigned to it an exception will be thrown.
// *
// * @param name The name of the parameter to get the value for
// * @param optional Is this value an optional parameter (default = false)
// * @param defaultValue The default value to return if this value is missing (default = "")
// * @return The value as a double
// */
//double ParameterList::getDouble(string name, bool optional, double defaultValue) {
//
//  double dReturn = defaultValue;
//
//  // Get the string
//  string sValue = getString(name, optional);
//  // If we have one
//  if (sValue != "")
//    dReturn = util::To<double>(sValue);
//
//  return dReturn;
//}
//
///**
// * This method will return the int value assigned to the parameter
// * <name>. If the value isn't there or if it has more than one value
// * assigned to it an exception will be thrown.
// *
// * @param name The name of the parameter to get the value for
// * @param optional Is this value an optional parameter (default = false)
// * @param defaultValue The default value to return if this value is missing (default = "")
// * @return The value as a int
// */
//int ParameterList::getInt(string name, bool optional, int defaultValue) {
//
//  int iReturn = defaultValue;
//
//  // Get the string
//  string sValue = getString(name, optional);
//  // If we have one
//  if (sValue != "")
//    iReturn = util::To<int>(sValue);
//
//  return iReturn;
//}
//
//unsigned ParameterList::getUnsigned(string name, bool optional, unsigned defaultValue) {
//  unsigned  iReturn = defaultValue;
//
//  // Get the string
//  string sValue = getString(name, optional);
//  // If we have one
//  if (sValue != "")
//    iReturn = util::To<unsigned>(sValue);
//
//  return iReturn;
//}
//
///**
// * This method will return the bool value assigned to the parameter
// * <name>. If the value isn't there or if it has more than one value
// * assigned to it an exception will be thrown.
// *
// * @param name The name of the parameter to get the value for
// * @param optional Is this value an optional parameter (default = false)
// * @param defaultValue The default value to return if this value is missing (default = "")
// * @return The value as a bool
// */
//bool ParameterList::getBool(string name, bool optional, bool defaultValue) {
//
//  string value = getString(name, optional);
//
//  // Check for non-existent optional value. If so, return default
//  if ( (value == "") && (optional) )
//    return defaultValue;
//
//  // Convert to lower case for matching
//  string sValue = value;
//  for (unsigned i = 0; i < sValue.length(); ++i)
//    sValue[i] = tolower(sValue[i]);
//  // Check for False/F match
//  if ( (sValue == CONFIG_FALSE) || (sValue == CONFIG_FALSE_SHORT) )
//    return false;
//  // Check for true/T match
//  if ( (sValue == CONFIG_TRUE) || (sValue == CONFIG_TRUE_SHORT) )
//    return true;
//
//  THROW_EXCEPTION(value + " is not a valid boolean value: " + name);
//  return true;
//}
//
///**
// * This method will fill the vector parameter <list> with the
// * values stored in parameter <name>
// *
// * @param &list The vector to fill with values
// * @param name The name of the parameter to load values from
// * @param optional If this parameter is optional (default = false);
// */
//void ParameterList::fillVector(vector<string> &list, string name, bool optional) {
//  try {
//    if (!hasParameter(name)) {
//      if (optional) {
//        return;
//      } else {
//        THROW_EXCEPTION("Could not find parameter: " + name);
//      }
//    }
//
//    // Clear the List
//    list.clear();
//
//    vector<string>::iterator vPtr = parameters_[name].begin();
//    while (vPtr != parameters_[name].end()) {
//      list.push_back((*vPtr));
//      vPtr++;
//    }
//  } catch (const string &ex) {
//    RETHROW_EXCEPTION(ex);
//  }
//}
//
///**
// * This method will fill the vector parameter <list> with the
// * values stored in parameter <name>
// *
// * @param &list The vector to fill with values
// * @param name The name of the parameter to load values from
// * @param optional If this parameter is optional (default = false);
// */
//void ParameterList::fillVector(vector<double> &list, string name, bool optional) {
//  if (!hasParameter(name)) {
//    if (optional) {
//      return;
//    } else {
//      THROW_EXCEPTION("Could not find parameter: " + name);
//    }
//  }
//
//  // Clear List
//  list.clear();
//
//  vector<string>::iterator vPtr = parameters_[name].begin();
//  while (vPtr != parameters_[name].end()) {
//    list.push_back(util::To<double>(*vPtr));
//    vPtr++;
//  }
//}
//
///**
// * This method will fill the vector parameter <list> with the
// * values stored in parameter <name>
// *
// * @param &list The vector to fill with values
// * @param name The name of the parameter to load values from
// * @param optional If this parameter is optional (default = false);
// */
//void ParameterList::fillVector(vector<int> &list, string name, bool optional) {
//  if (!hasParameter(name)) {
//    if (optional) {
//      return;
//    } else {
//      THROW_EXCEPTION("Could not find parameter: " + name);
//    }
//  }
//
//  // Clear the list
//  list.clear();
//
//  vector<string>::iterator vPtr = parameters_[name].begin();
//  while (vPtr != parameters_[name].end()) {
//    list.push_back(util::To<int>(*vPtr));
//    vPtr++;
//  }
//}
//
///**
// * This method will fill the vector parameter <list> with the
// * values stored in parameter <name>
// *
// * @param &list The vector to fill with values
// * @param name The name of the parameter to load values from
// * @param optional If this parameter is optional (default = false);
// */
//void ParameterList::fillVector(vector<bool> &list, string name, bool optional) {
//  if (!hasParameter(name)) {
//    if (optional) {
//      return;
//    } else {
//      THROW_EXCEPTION("Could not find parameter: " + name);
//    }
//  }
//
//  // Clear the list
//  list.clear();
//
//  vector<string>::iterator vPtr = parameters_[name].begin();
//  while (vPtr != parameters_[name].end()) {
//    list.push_back(util::To<bool>(*vPtr));
//    vPtr++;
//  }
//}
//
///**
// * This method will fill the parameter <list> with the column that have
// * been defined for table <tableName>.
// *
// * @param &list The vector to fill with the column names
// * @param tableName The name of the table to get the columns from
// * @param tableIndex The nth (0-N) table to get this information from (default = 0)
// */
//void ParameterList::fillTableColumnVector(vector<string> &list, string tableName, unsigned tableIndex) {
//
//  list.clear();
//
//  if (tables_.find(tableName) == tables_.end())
//    THROW_EXCEPTION("Table not found: " + tableName);
//
//  if (tableIndex >= tables_[tableName].size())
//    THROW_EXCEPTION("tableIndex Parameter is larger than vector size index");
//
//  // Assign the values to our parameter
//  list.assign( tables_[tableName][tableIndex].columns_.begin(), tables_[tableName][tableIndex].columns_.end());
//
//}
///**
// * This method will fill the parameter <list> with the data that has
// * been defined for the table <tableName>
// *
// * @param &list The vector to fill with the table data
// * @param tableName the name of the table to load the information from
// * @param tableIndex the nth (0-N) table to get this information from (default = 0)
// */
//void ParameterList::fillTableDataVector(vector<string> &list, string tableName, unsigned tableIndex) {
//  list.clear();
//
//  if (tables_.find(tableName) == tables_.end())
//    THROW_EXCEPTION("Table not found: " + tableName);
//
//  if (tableIndex >= tables_[tableName].size())
//    THROW_EXCEPTION("tableIndex Parameter is larger than vector size index");
//
//  // Assign the values to our parameter
//  vector<string>::iterator iter = tables_[tableName][tableIndex].data_.begin();
//  for (; iter != tables_[tableName][tableIndex].data_.end(); iter++) {
//
//    list.push_back((*iter));
//  }
//}
//
///**
// * This method clears any assigned values in the parameter list.
// * It does NOT clear out any of the registerAllowed() values though.
// *
// * This is mostly used for unit testing in fixture tearDown()
// */
//void ParameterList::clear() {
//  parameters_.clear();
//  tables_.clear();
//}

} /* namespace isam */
