/**
 * @file ObjectiveFunction.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 1/09/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * TODO: Add documentation
 */
#ifndef ASSERTS_OBJECTIVEFUNCTION_H_
#define ASSERTS_OBJECTIVEFUNCTION_H_

// headers
#include "../../Asserts/Assert.h"

// namespaces
namespace niwa {
namespace asserts {

// classes
class ObjectiveFunction : public niwa::Assert {
public:
  // methods
  ObjectiveFunction(shared_ptr<Model> model);
  virtual ~ObjectiveFunction() = default;
  void Execute() override final;

protected:
  // methods
  void DoValidate(){};
  void DoBuild() override final;

private:
  // members
  Double value_;
};

} /* namespace asserts */
} /* namespace niwa */

#endif /* ASSERTS_OBJECTIVEFUNCTION_H_ */
