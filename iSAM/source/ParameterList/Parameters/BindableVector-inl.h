/**
 * @file BindableVector-inl.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 31/10/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifndef PARAMETERS_BINDABLEVECTOR_INL_H_
#define PARAMETERS_BINDABLEVECTOR_INL_H_

// headers
#include "Utilities/To.h"
#include "Utilities/Logging/Logging.h"

// namespaces
namespace isam {
namespace parameters {

/**
 * default constructor
 *
 * @param label The label of the parameter
 * @param target The target variable in the model to bind to this parameter
 * @param description A text description of the parameter for the help system
 */
template<typename T>
BindableVector<T>::BindableVector(const string& label, vector<T>* target, const string& description)
: Parameter(label, description) {

  target_ = target;
}

/**
 * This method will bind our string values to the target value doing the proper
 * type checking etc.
 */
template<typename T>
void BindableVector<T>::Bind() {
  if (values_.size() == 0 && !is_optional_)
    LOG_ERROR(location() << " is a required parameter. Please ensure you have defined values for it in the configuration file");

  T value;
  for (unsigned i = 0; i < values_.size(); ++i) {
    if (!isam::utilities::To<T>(values_[i], value))
      LOG_ERROR(location() << " value " << values_[i] << " could not be converted to the proper type. Please check you have defined it properly");

    target_->push_back(value);
  }
}

} /* namespace parameters */
} /* namespace isam */

#endif /* BINDABLEVECTOR_INL_H_ */
