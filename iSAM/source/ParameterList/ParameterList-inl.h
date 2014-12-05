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
#ifndef PARAMETERLIST_INL_H_
#define PARAMETERLIST_INL_H_

// Headers
#include <boost/lexical_cast.hpp>
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
void ParameterList::Bind(const string& label, T* target, const string& description, const string& values) {
  boost::shared_ptr<Bindable<T> > parameter = boost::shared_ptr<Bindable<T> >(new Bindable<T>(label, target, description));
  parameters_[label] = parameter;
}

/**
 *
 */
template<typename T>
void ParameterList::Bind(const string& label, T* target, const string& description, const string& values, T default_value) {
  boost::shared_ptr<Bindable<T> > parameter = boost::shared_ptr<Bindable<T> >(new Bindable<T>(label, target, description));
  parameter->set_is_optional(true);
  parameter->set_default_value(default_value);
  parameters_[label] = parameter;
}

/**
 *
 */
template<typename T>
void ParameterList::Bind(const string& label, vector<T>* target, const string& description, const string& values, bool optional) {
  boost::shared_ptr<BindableVector<T> > parameter = boost::shared_ptr<BindableVector<T> >(new BindableVector<T>(label, target, description));
  parameter->set_is_optional(optional);
  parameters_[label] = parameter;
}

} /* namespace niwa */
#endif /* PARAMETERLIST_INL_H_ */
