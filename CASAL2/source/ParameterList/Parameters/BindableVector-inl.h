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
#include <vector>

#include "Utilities/String.h"
#include "Utilities/To.h"
#include "Logging/Logging.h"

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
  T value = T();
  for (unsigned i = 0; i < values_.size(); ++i) {
    if (!niwa::utilities::To<T>(values_[i], value))
      LOG_ERROR() << location() << ": " << label_ << " value " << values_[i] << " could not be converted to type "
        << utilities::demangle(typeid(value).name());

      target_->push_back(value);
  }

  /**
   * Check that the values provided are in our allowed_values_ object if it has
   * been defined
   */
  if (allowed_values_.size() > 0) {
    for (T value : *target_) {
      if (std::find(allowed_values_.begin(), allowed_values_.end(), value) == allowed_values_.end())
        LOG_ERROR() << location() << " value " << value << " is not in the allowed values list: "
            << utilities::String::join<T>(allowed_values_, ", ");
    }
  }

  /**
   * Check that the values provided are within the ranges provided
   */
  if (range_.lower_flagged_ || range_.upper_flagged_) {
    for (T value : *target_) {
      if (range_.lower_flagged_ && (value < range_.lower_bound_ || (value == range_.lower_bound_ && !range_.lower_inclusive_)))
        LOG_ERROR() << location() << " value " << value << " exceeds the lower bound: " << range_.lower_bound_;
      if (range_.upper_flagged_ && (value > range_.upper_bound_ || (value == range_.upper_bound_ && !range_.upper_inclusive_)))
        LOG_ERROR() << location() << " value " << value << " exceeds the upper bound: " << range_.upper_bound_;
    }
  }
  if (allowed_values_.size() != 0) {
    for (T value : *target_) {
      if (std::find(allowed_values_.begin(), allowed_values_.end(), value) == allowed_values_.end())
        LOG_ERROR() << location() << " value " << value << " is not in the allowed values list: "
          << utilities::String::join(allowed_values_, ", ");
    }
  }
}

/**
 * This method sets a list of allowed values that can be defined for this
 * parameter.
 *
 * @param list A list of values that are allowed for this parameter
 */
template<typename T>
void BindableVector<T>::set_allowed_values(std::initializer_list<T> list) {
  allowed_values_.assign(list.begin(), list.end());
}

/**
 * Override set_range() with std::string and throw an error because we do not allow range based
 * checks to be done on strings.
 */
//template<>
//void BindableVector<string>::set_range(string lower_bound, string upper_bound, bool lower_inclusive, bool upper_inclusive) {
//  LOG_CODE_ERROR(location() << " cannot call set_range() with a string defined parameter");
//}

/**
 * This method sets an enforced value range on the objects stored within this parameter.
 * This will be checked during the bind method and an error will be thrown if it's not
 * acceptable.
 *
 * inclusive means the value being specified as the lower bound is also a valid value.
 * e.g. lower_bound 0 and inclusive means value >= 0 is ok, but value < 0 is not
 * a lower bound 0 not inclusive means value > 0 is ok, but value <= 0 is not.
 *
 * @param lower_bound The lowest the value can be (default inclusive)
 * @param upper_bound The highest the value can be (default inclusive)
 * @param lower_inclusive Is the lower bound value inclusive (default true)
 * @param upper_inclusie Is the upper bound value inclusive (default true)
 */
template<typename T>
void BindableVector<T>::set_range(T lower_bound, T upper_bound, bool lower_inclusive, bool upper_inclusive) {
  range_.lower_flagged_ = true;
  range_.upper_flagged_ = true;
  range_.lower_bound_ = lower_bound;
  range_.upper_bound_ = upper_bound;
  range_.lower_inclusive_ = lower_inclusive;
  range_.upper_inclusive_ = upper_inclusive;
}

/**
 * This method will set an enforced lower bound only
 *
 * inclusive means the value being specified as the lower bound is also a valid value.
 * e.g. lower_bound 0 and inclusive means value >= 0 is ok, but value < 0 is not
 * a lower bound 0 not inclusive means value > 0 is ok, but value <= 0 is not.
 *
 * @param lower_bound The lower bound to set
 * @param inclusive Is the lower bound inclusive or exclusive
 */
template<typename T>
void BindableVector<T>::set_lower_bound(T lower_bound, bool inclusive) {
  range_.lower_flagged_ = true;
  range_.lower_bound_ = lower_bound;
  range_.lower_inclusive_ = inclusive;
}

/**
 * This method sets an enforced upper bound and inclusive flag
 *
 * inclusive means the value being specified as the lower bound is also a valid value.
 * e.g. lower_bound 0 and inclusive means value >= 0 is ok, but value < 0 is not
 * a lower bound 0 not inclusive means value > 0 is ok, but value <= 0 is not.
 *
 * @param upper_bound the Upper bound to set for the parameter
 * @param inclusive is the upper bound inclusive or exclusive
 */
template<typename T>
void BindableVector<T>::set_upper_bound(T upper_bound, bool inclusive) {
  range_.upper_flagged_ = true;
  range_.upper_bound_ = upper_bound;
  range_.upper_inclusive_ = inclusive;
}

/**
 * Build a vector of values for this parameter
 */
template<typename T>
vector<string> BindableVector<T>::current_values() {
  vector<string> result;

  for (T value : (*target_))
    result.push_back(utilities::ToInline<T, string>(value));

  return result;
}

} /* namespace parameters */
} /* namespace niwa */

#endif /* BINDABLEVECTOR_INL_H_ */
