/**
 * @file ParameterList-inl.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 19/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PARAMETERLIST_PARAMETER_INL_H_
#define PARAMETERLIST_PARAMETER_INL_H_

// Headers
#include <typeinfo>

#include "Utilities/To.h"
#include "Utilities/Types.h"
#include "Utilities/Logging/Logging.h"

// Namespaces
namespace isam {
namespace parameterlist {

using isam::utilities::Double;

/**
 *
 */
template<typename T>
T Parameter::GetValue() const {

  if (values_.size() == 0) {
    LOG_CODE_ERROR("Trying to call .GetValue() on a parameter with no values. Perhaps you need to specify a default_value for it? Label: " << label_);
  }
  if (values_.size() != 1) {
    LOG_ERROR("At line " << line_number_ << " in file " << file_name_
        << ": " << label_ << " can only have 1 parameter specified. E.g " << label_ << " <value>");
  }

  RequireValueType<T>();

  T result_value;
  if (!isam::utilities::To<T>(values_[0], result_value)) {
    LOG_CODE_ERROR("Failed to convert type. This shouldn't occur because we did a type check above");
  }

  return result_value;
}

/**
 *
 */
template<typename T>
T Parameter::GetValue(T default_value) const {

  if (values_.size() == 0)
    return default_value;

  if (values_.size() != 1) {
    LOG_ERROR("At line " << line_number_ << " in file " << file_name_
        << ": " << label_ << " can only have 1 parameter specified. E.g " << label_ << " <value>");
  }

  RequireValueType<T>();

  T result_value = default_value;
  if (!isam::utilities::To<T>(values_[0], result_value)) {
    LOG_CODE_ERROR("Failed to convert type. This shouldn't occur because we did a type check above");
  }

  return result_value;
}

/**
 *
 */
template<typename T>
vector<T> Parameter::GetValues() const {

  RequireValueType<T>();

  vector<T> result_value;

  for (size_t i = 0; i < values_.size(); ++i) {
    T temp_value;

    if (!isam::utilities::To<T>(values_[i], temp_value)) {
      LOG_CODE_ERROR("Failed to convert type. This shouldn't occur because we did a type check above");
    }

    result_value.push_back(temp_value);
  }

  return result_value;
}

/**
 *
 */
template<>
inline vector<Double> Parameter::GetValues() const {

  RequireValueType<double>();

  vector<Double> result_value;

  for (size_t i = 0; i < values_.size(); ++i) {
    double temp_value;

    if (!isam::utilities::To<double>(values_[i], temp_value)) {
      LOG_CODE_ERROR("Failed to convert type. This shouldn't occur because we did a type check above");
    }

    result_value.push_back(temp_value);
  }

  return result_value;
}

/**
 *
 */
//template<typename SOURCE, typename TARGET>
//vector<TARGET> Parameter::GetValues() const {
//
//  RequireValueType<SOURCE>();
//
//  vector<TARGET> result_value;
//
//  for (size_t i = 0; i < values_.size(); ++i) {
//    SOURCE temp_value;
//
//    if (!isam::utilities::To<SOURCE>(values_[i], temp_value)) {
//      LOG_CODE_ERROR("Failed to convert type. This shouldn't occur because we did a type check above");
//    }
//
//    result_value.push_back(temp_value);
//  }
//
//  return result_value;
//}

/**
 *
 */
template<typename T>
void Parameter::RequireValueType() const {
  T result_value;

  for (size_t i = 0; i < values_.size(); ++i) {
    if (!isam::utilities::To<T>(values_[i], result_value)) {
      LOG_ERROR("At line " << line_number_ << " in file " << file_name_
          << ": " << label_ << " cannot be interpreted as type " << typeid(T).name() << " because the value " << values_[i] << " is invalid");
    }
  }
}

/**
 * Specialisation: unsigned
 */
template<>
inline void Parameter::RequireValueType<unsigned>() const {
  unsigned result_value;

  for (size_t i = 0; i < values_.size(); ++i) {
    if (!isam::utilities::To<unsigned>(values_[i], result_value)) {
      LOG_ERROR("At line " << line_number_ << " in file " << file_name_
          << ": " << label_ << " cannot be interpreted as a positive number because the value " << values_[i] << " is invalid");
    }
  }
}


} /* namespace parameterlist */
} /* namespace isam */
#endif /* PARAMETERLIST_PARAMETER_INL_H_ */
