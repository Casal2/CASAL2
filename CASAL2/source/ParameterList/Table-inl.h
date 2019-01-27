/**
 * @file Table-inl.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/10/2018
 * @section LICENSE
 *
 * Copyright NIWA Science ©2018 - www.niwa.co.nz
 *
 */
#ifndef SOURCE_PARAMETERLIST_TABLE_INL_H_
#define SOURCE_PARAMETERLIST_TABLE_INL_H_

#include <vector>

#include "Utilities/To.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {
namespace parameters {

using std::vector;

/**
 * This method will run through all of the data in a specific column ensuring every value is numeric.
 * It'll throw up appropriate error if not. This makes validation of tables easier.
 *
 * @param column The column header to check data for
 */
template<typename T>
void Table::CheckColumnValuesAreType(const string& column) {
  unsigned index = column_index(column);
  T value;

  for (const auto& row : data_) {
    if (!utilities::To<string, T>(row[index], value))
      LOG_ERROR() << location() << "The value" << row[index] << " in column " << column << " could not be converted to type " << utilities::demangle(typeid(value).name());
  }
}

/**
 * This method will go through every value in a column and check to ensure it's present
 * in the values parameter being passed in. This will easily allow us to pass in a set
 * of values to be verified.
 *
 * @param column The label for the column to process
 * @param values A vector of values to check each column value against
 */
template<typename T>
void Table::CheckColumnValuesContain(const string& column, const vector<T>& values) {
  unsigned index = column_index(column);
  vector<T> table_values;
  table_values.reserve(data_.size());
  T value;

  for (const auto& row : data_) {
    if (!utilities::To<string, T>(row[index], value))
      LOG_ERROR() << location() << "The value" << row[index] << " in column " << column << " could not be converted to type " << utilities::demangle(typeid(value).name());

    table_values.push_back(value);
  }

  for (const auto& table_value : table_values) {
    if (std::find(values.begin(), values.end(), table_value) == values.end())
      LOG_ERROR() << location() << "The value " << table_value << " in column " << column << " is not a valid value for this model";
  }
}

/**
 * This method will return all values in a column in a single vector
 * as the target type. This makes it easy for objects using this table
 * to do something like table->GetColumnValuesAs<unsigned>(PARAM_YEAR);
 *
 * @param column The column label to prcess
 */
template<typename T>
vector<T> Table::GetColumnValuesAs(const string& column) {
  vector<T> result;
  result.reserve(data_.size());
  T value;

  unsigned index = column_index(column);
  for (const auto& row : data_) {
    if (!utilities::To<string, T>(row[index], value))
      LOG_ERROR() << location() << "The value" << row[index] << " in column " << column << " could not be converted to type " << utilities::demangle(typeid(value).name());

    result.push_back(value);
  }

  return result;
}

template<>
inline vector<string> Table::GetColumnValuesAs(const string& column) {
  vector<string> result;

  unsigned index = column_index(column);
  for (const auto& row : data_) {
    result.push_back(row[index]);
  }
  return result;
}

} /* namespace parameters */
} /* namespace niwa */

#endif /* SOURCE_PARAMETERLIST_TABLE_INL_H_ */
