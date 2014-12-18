/**
 * @file Bindable-inl.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/10/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifndef PARAMETERS_BINDABLE_INL_H_
#define PARAMETERS_BINDABLE_INL_H_

// headers
#include <boost/algorithm/string/join.hpp>
#include <vector>

#include "Utilities/To.h"
#include "Utilities/Logging/Logging.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {
namespace parameters {

using std::vector;

/**
 * default constructor
 *
 * @param label The label of the parameter
 * @param target The target variable in the model to bind to this parameter
 * @param description A text description of the parameter for the help system
 */
template<typename T>
Bindable<T>::Bindable(const string& label, T* target, const string& description)
: Parameter(label, description) {

  target_ = target;
}

/**
 * This method will bind our string values to the target value doing the proper
 * type checking etc.
 */
template<typename T>
void Bindable<T>::Bind() {
  if (values_.size() > 1)
    LOG_ERROR(location() << ": " << label_ << " only supports having a single value defined. There are  " << values_.size() << " values defined.\n"
        << "The values defined are: " << boost::algorithm::join(values_, " | "));

  if (values_.size() > 0) {
    if (!niwa::utilities::To<T>(values_[0], *target_))
      LOG_ERROR(location() << ": " << label_ << " value " << values_[0] << " could not be converted to type " << utilities::demangle(typeid(*target_).name()) << ". Please check you have defined it properly.");
  } else if (is_optional_) {
    *target_ = default_value_;
  } else
    LOG_CODE_ERROR(location() << ": " << label_ << "Parameter not defined or missing");
}

/**
 * Build a vector of values for this parameter
 */
template<typename T>
vector<string> Bindable<T>::GetCurrentValues() {
  vector<string> result;
  result.push_back(utilities::ToInline<T, string>(*target_));
  return result;
}

} /* namespace parameters */
} /* namespace niwa */

#endif /* BINDABLE_INL_H_ */
