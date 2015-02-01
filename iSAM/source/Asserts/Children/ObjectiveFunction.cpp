/**
 * @file ObjectiveFunction.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 1/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "ObjectiveFunction.h"

#include "Model/Model.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
// namespaces
namespace niwa {
namespace asserts {

/**
 *
 */
ObjectiveFunction::ObjectiveFunction() {
  parameters_.Bind<Double>(PARAM_VALUE, &value_, "Expected value of the objective function", "");
}

/**
 *
 */
void ObjectiveFunction::DoBuild() {
  Model::Instance()->Subscribe(State::kFinalise, shared_ptr());
}

/**
 *
 */
void ObjectiveFunction::Execute() {
  niwa::ObjectiveFunction& obj = niwa::ObjectiveFunction::Instance();
  if (abs(AS_DOUBLE(value_) - AS_DOUBLE(obj.score())) > 1e-9)
    LOG_ERROR("Assert Failure: Objective Function had actual value " << obj.score() << " when we expected " << value_
        << " with difference: " << abs(AS_DOUBLE(value_) - AS_DOUBLE(obj.score())));
}

} /* namespace asserts */
} /* namespace niwa */
