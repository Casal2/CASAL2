/**
 * @file Table-inl.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/07/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Template methods for our parameter table
 */
#ifndef SOURCE_PARAMETERLIST_TABLE_INL_H_
#define SOURCE_PARAMETERLIST_TABLE_INL_H_

#include <vector>
#include <map>
#include <string>

#include "Logging/Logging.h"
#include "Utilities/To.h"
#include "Utilities/Types.h"

namespace niwa {
namespace parameters {

using std::string;
using std::vector;
using std::map;

/**
 *
 */
template<typename MasterKey, typename Key, typename Value>
bool Table::Populate(const MasterKey& master_key_column, const vector<Key>& keys, map<MasterKey, map<Key, Value>>& out_matrix) {

  /**
   * Find the master column so we can index things properly
   */
  string master_key_string = utilities::ToInline<MasterKey, string>(master_key_column);
  if (std::find(columns_.begin(), columns_.end(), master_key_string) == columns_.end()) {
    LOG_ERROR() << location() << "column " << master_key_string << " could not be found in the table " << label_;
    return false;
  }
  unsigned master_key_index = this->column_index(master_key_string);

  /**
   * Build a map of the key indexes so we can do a quick lookup on where the data will be stored
   */
  map<Key, unsigned> key_indexes;
  for (Key key_value : keys) {
    key_indexes[key_value] = this->column_index(utilities::ToInline<Key, string>(key_value));

    if (key_indexes.find(key_value) != key_indexes.end()) {
      LOG_ERROR() << location() << "column " << key_value << " is defined twice in the table " << label_;
      return false;
    }

    if (key_indexes[key_value] == columns_.size()) {
      LOG_ERROR() << location() << "column " << key_value << " could not be found in the table " << label_;
      return false;
    }
  }

  /**
   * Iterate through the rows and start to populate the data
   */
  for (auto row : data_) {
    MasterKey master_key = MasterKey();
    if (!utilities::To<string, MasterKey>(row[master_key_index], master_key)) {
      LOG_ERROR() << "X";
      return false;
    }

    for (auto key_index : key_indexes) {
      Key key_value = Key();
      if (!utilities::To<Key>(key_index.first, key_value)) {
        LOG_ERROR() << location() << "";
        return false;
      }

      Value value = Value();
      if (!utilities::To<Value>(row[key_index.second], value)) {
        LOG_ERROR() << location() << "value " << row[key_index.second] << " could not be converted to " << utilities::demangle(typeid(value).name());
        return false;
      }
      out_matrix[master_key][key_value] = value;
    }
  }

  return true;
}

/**
 *
 */
//template<typename Values>
//bool Table::Populate(const string& column, vector<Values> &data) {
//  if (std::find(columns_.begin(), columns_.end(), column) == columns_.end()) {
//    LOG_ERROR() << location() << " column " << column << " could not be found to populate";
//    return false;
//  }
//  unsigned column_index = this->column_index(column);
//
//  for (auto row : this->data()) {
//    Values temp = Values();
//    if (!utilities::To<Values>(row[column_index], temp)) {
//      LOG_ERROR() << location() << " value " << row[column_index] << " could not be converted to " << utilities::demangle(typeid(temp).name());
//      return false;
//    }
//
//    data.push_back(row[column_index]);
//  }
//
//  return true;
//}
//
///**
// * This method will populate a map of values using
// * the column name as the key
// */
//template<typename Key, typename Values>
//bool Table::Populate(const vector<Key>& columns, map<Key,vector<Values>> &data) {
//
//  for (auto column : columns) {
//    // get column index
//    string column_label = utilities::ToInline<Key, string>(column);
//
//    if (std::find(columns_.begin(), columns_.end(), column_label) == columns_.end()) {
//      LOG_ERROR() << location() << " column " << column_label << " could not be found to populate";
//      return false;
//    }
//
//    unsigned column_index = this->column_index(column_label);
//
//    for (auto row : this->data()) {
//      Values temp = Values();
//      if (!utilities::To<Values>(row[column_index], temp)) {
//        LOG_ERROR() << location() << " value " << row[column_index] << " could not be converted to " << utilities::demangle(typeid(temp).name());
//        return false;
//      }
//
//      data[column].push_back(temp);
//    }
//  }
//
//  return true;
//}

} /* namespace parameters */
} /* namespace niwa */

#endif /* SOURCE_PARAMETERLIST_TABLE_INL_H_ */
