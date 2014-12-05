/**
 * @file Bindable.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/10/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a specialisation of our parameter class that is used when
 * binding parameters to configuration file values for objects.
 */
#ifndef PARAMETERS_BINDABLE_H_
#define PARAMETERS_BINDABLE_H_

// headers
#include <typeinfo>

#include "ParameterList/Parameter.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {
namespace parameters {

/**
 * class definition
 */
template<typename T>
class Bindable : public niwa::parameterlist::Parameter {
public:
  // methods
  Bindable(const string& label, T* target, const string& description);
  virtual                     ~Bindable() = default;
  void                        Bind() override final;
  void                        set_default_value(T value) { default_value_ = value; }
  string                      stored_type() const override final { return utilities::demangle(typeid(*target_).name()); }

private:
  // members
  T*                          target_;
  T                           default_value_;
};

} /* namespace parameters */
} /* namespace niwa */

// headers
#include "Bindable-inl.h"

#endif /* BINDABLE_H_ */
