/**
 * @file ParameterList-inl.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 19/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PARAMETERLIST_INL_H_
#define PARAMETERLIST_INL_H_

// Headers
#include <memory>
#include <iostream>
#include <exception>

#include "ParameterList/Parameters/Bindable.h"
#include "ParameterList/Parameters/BindableVector.h"
#include "../Utilities/Exception.h"

// Using
using std::cout;
using std::endl;
using niwa::parameters::Bindable;
using niwa::parameters::BindableVector;

namespace niwa {

/**
 *
 */
template<typename T>
Bindable<T>* ParameterList::Bind(const string& label, T* target, const string& description, const string& values) {
  if (parameters_.find(label) != parameters_.end()) {
    LOG_CODE_ERROR() << "The parameter " << label << " has already been bound with the description " << description;
  }
  Bindable<T>* parameter = new Bindable<T>(label, target, description);
  parameters_[label] = parameter;
  return parameter;
}

/**
 *
 */
template<typename T>
Bindable<T>* ParameterList::Bind(const string& label, T* target, const string& description, const string& values, T default_value) {
  if (parameters_.find(label) != parameters_.end()) {
    LOG_CODE_ERROR() << "The parameter " << label << " has already been bound with the description " << description;
  }
  Bindable<T>* parameter = new Bindable<T>(label, target, description);
  parameter->set_is_optional(true);
  parameter->set_default_value(default_value);
  parameters_[label] = parameter;
  return parameter;
}

/**
 *
 */
template<typename T>
BindableVector<T>* ParameterList::Bind(const string& label, vector<T>* target, const string& description, const string& values, bool optional) {
  if (parameters_.find(label) != parameters_.end()) {
    LOG_CODE_ERROR() << "The parameter " << label << " has already been bound with the description " << description;
  }
  BindableVector<T>* parameter = new BindableVector<T>(label, target, description);
  parameter->set_is_optional(optional);
  parameters_[label] = parameter;
  return parameter;
}

} /* namespace niwa */
#endif /* PARAMETERLIST_INL_H_ */
