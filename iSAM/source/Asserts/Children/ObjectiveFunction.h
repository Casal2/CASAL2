/**
 * @file ObjectiveFunction.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 1/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef ASSERTS_OBJECTIVEFUNCTION_H_
#define ASSERTS_OBJECTIVEFUNCTION_H_

// headers
#include "Asserts/Assert.h"

// namespaces
namespace niwa {
namespace asserts {

/**
 * class definition
 */
class ObjectiveFunction : public niwa::Assert {
public:
  // methods
  ObjectiveFunction();
  virtual                     ~ObjectiveFunction() = default;
  void                        DoValidate() { };
  void                        DoBuild() override final;
  void                        Execute() override final;

private:
  // members
  Double                      value_;
};

} /* namespace asserts */
} /* namespace niwa */

#endif /* ASSERTS_OBJECTIVEFUNCTION_H_ */
