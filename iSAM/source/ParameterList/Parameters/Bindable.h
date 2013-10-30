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
#include "ParameterList/Parameter.h"

// namespaces
namespace isam {
namespace parameters {

/**
 * class definition
 */
template<typename T>
class Bindable : public isam::parameterlist::Parameter {
public:
  // methods
  Bindable(const string& label, T& target, const string& description);
  virtual                     ~Bindable() = default;
  void                        Bind() override final;

private:
  // members
  T*                          target_;
};

} /* namespace parameters */
} /* namespace isam */

// headers
#include "Bindable-inl.h"

#endif /* BINDABLE_H_ */
