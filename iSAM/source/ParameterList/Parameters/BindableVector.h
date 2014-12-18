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
#include "Utilities/Types.h"

// namespaces
namespace niwa {
namespace parameters {

using std::vector;

/**
 * class definition
 */
template<typename T>
class BindableVector : public niwa::parameterlist::Parameter {
public:
  // methods
  BindableVector(const string& label, vector<T>* target, const string& description);
  virtual                     ~BindableVector() = default;
  void                        Bind() override final;
  string                      stored_type() const override final { return utilities::demangle(typeid(*target_).name()); }
  vector<string>              GetCurrentValues() override final;

private:
  // members
  vector<T>*                  target_;
};

} /* namespace parameters */
} /* namespace niwa */

// headers
#include "BindableVector-inl.h"

#endif /* BINDABLEVECTOR_H_ */
