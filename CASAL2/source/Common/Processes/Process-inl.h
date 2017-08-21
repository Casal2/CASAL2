/**
 * @file Process-inl.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */
#ifndef SOURCE_PROCESSES_PROCESS_INL_H_
#define SOURCE_PROCESSES_PROCESS_INL_H_

// headers
#include <string>
#include <vector>
#include <map>

#include "Common/Utilities/To.h"

// namespaces
namespace niwa {
using std::string;
using std::vector;
using std::map;

/**
 * This method converts the parameter value into a string
 * and stores it to be printed in the process report
 * if one has been requested
 *
 * @param label The label of the parameter to print
 * @param value The value to add to the parameter
 */
template<typename T>
void Process::StoreForReport(const string& label, T value) {
  if (!print_report_ && !model_->global_configuration().print_tabular())
      return;

  string svalue = utilities::ToInline<T, string>(value);
  print_values_[label].push_back(svalue);
}

/**
 * This method converts the parameter value into a string
 * and stores it to be printed in the process report
 * if one has been requested
 *
 * @param label The label of the parameter to print
 * @param value The values to add to the parameter
 */
template<typename T>
void Process::StoreForReport(const string& label, const vector<T>& values) {
  if (!print_report_ && !model_->global_configuration().print_tabular())
    return;

  string svalue = "";
  for (auto value : values) {
    svalue = utilities::ToInline<T, string>(value);
    print_values_[label].push_back(values);
  }
}

/**
 * This method converts the parameter value into a string
 * and stores it to be printed in the process report when --tabular is on
 * if one has been requested
 *
 * @param label The label of the parameter to print
 * @param value The value to add to the parameter
 */
template<typename T>
void Process::StoreForTabularReport(const string& label, T value) {
  if (!print_report_ && model_->global_configuration().print_tabular())
      return;

  string svalue = utilities::ToInline<T, string>(value);
  print_tabular_values_[label] = svalue;
}

/**
 * This method converts the parameter value into a string
 * and stores it to be printed in the process report when --tabular is on
 * if one has been requested
 *
 * @param label The label of the parameter to print
 * @param value The values to add to the parameter
 */
template<typename T>
void Process::StoreForTabularReport(const string& label, const vector<T>& values) {
  if (!print_report_ && model_->global_configuration().print_tabular())
    return;

  string svalue = "";
  for (auto value : values) {
    svalue = utilities::ToInline<T, string>(value);
    print_tabular_values_[label] = values;
  }
}


}

#endif /* SOURCE_PROCESSES_PROCESS_INL_H_ */
