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

#include "Utilities/String.h"
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

  /**
   * Check if the value provided is within the allowed values (if they have been defined)
   */
  if (allowed_values_.size() > 0) {
    if (std::find(allowed_values_.begin(), allowed_values_.end(), *target_) == allowed_values_.end())
      LOG_ERROR(location() << " value " << *target_ << " is not in the allowed values list: "
          << utilities::String::join<T>(allowed_values_, ", "));
  }

  /**
   * Check if the value provided is within the ranges provided (if defined)
   * or the allowed values
   */
  if (range_.lower_flagged_ || range_.upper_flagged_) {
    if (range_.lower_flagged_ && (*target_ < range_.lower_bound_ || (*target_ == range_.lower_bound_ && !range_.lower_inclusive_)))
      LOG_ERROR(location() << " value " << *target_ << " exceeds the lower bound: " << range_.lower_bound_);
    if (range_.upper_flagged_ && (*target_ > range_.upper_bound_ || (*target_ == range_.upper_bound_ && !range_.upper_inclusive_)))
      LOG_ERROR(location() << " value " << *target_ << " exceeds the upper bound: " << range_.upper_bound_);
  }
  if (allowed_values_.size() != 0) {
    if (std::find(allowed_values_.begin(), allowed_values_.end(), *target_) == allowed_values_.end())
      LOG_ERROR(location() << " value " << *target_ << " is no in the list of allowed values: " << utilities::String::join(allowed_values_, ", "));
  }
}

/**
 * This method sets a list of allowed values that can be defined for this
 * parameter.
 *
 * @param list A list of values that are allowed for this parameter
 */
template<typename T>
void Bindable<T>::set_allowed_values(std::initializer_list<T> list) {
  allowed_values_.assign(list.begin(), list.end());
}

/**
 * This method sets an enforced value range on the objects stored within this parameter.
 * This will be checked during the bind method and an error will be thrown if it's not
 * acceptable.
 *
 * @param lower_bound The lowest the value can be (default inclusive)
 * @param upper_bound The highest the value can be (default inclusive)
 * @param lower_inclusive Is the lower bound value inclusive (default true)
 * @param upper_inclusie Is the upper bound value inclusive (default true)
 */
template<typename T>
void Bindable<T>::set_range(T lower_bound, T upper_bound, bool lower_inclusive, bool upper_inclusive) {
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
 * @param lower_bound The lower bound to set
 * @param inclusive Is the lower bound inclusive or exclusive
 */
template<typename T>
void Bindable<T>::set_lower_bound(T lower_bound, bool inclusive) {
  range_.lower_flagged_ = true;
  range_.lower_bound_ = lower_bound;
  range_.lower_inclusive_ = inclusive;
}

/**
 * This method sets an enforced upper bound and inclusive flag
 *
 * @param upper_bound the Upper bound to set for the parameter
 * @param inclusive is the upper bound inclusive or exclusive
 */
template<typename T>
void Bindable<T>::set_upper_bound(T upper_bound, bool inclusive) {
  range_.upper_flagged_ = true;
  range_.upper_bound_ = upper_bound;
  range_.upper_inclusive_ = inclusive;
}

/**
 * Build a vector of values for this parameter
 */
template<typename T>
vector<string> Bindable<T>::current_values() {
  vector<string> result;
  result.push_back(utilities::ToInline<T, string>(*target_));
  return result;
}

} /* namespace parameters */
} /* namespace niwa */

#endif /* BINDABLE_INL_H_ */
