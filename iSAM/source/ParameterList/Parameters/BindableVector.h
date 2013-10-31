/**
 * @file BindableVector.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/10/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class i
 */
#ifndef PARAMETERS_BINDABLEVECTOR_H_
#define PARAMETERS_BINDABLEVECTOR_H_

// headers
#include <vector>

#include "ParameterList/Parameter.h"

// namespaces
namespace isam {
namespace parameters {

using std::vector;

/**
 * class definition
 */
template<typename T>
class BindableVector : public isam::parameterlist::Parameter {
public:
  // methods
  BindableVector(const string& label, vector<T>* target, const string& description);
  virtual                     ~BindableVector() = default;
  void                        Bind() override final;

private:
  // members
  vector<T>*                  target_;
};

} /* namespace parameters */
} /* namespace isam */

// headers
#include "BindableVector-inl.h"

#endif /* BINDABLEVECTOR_H_ */
