/**
 * @file ObjectiveFunction.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 1/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "ObjectiveFunction.h"

#include <iomanip>   // std::setprecision
#include <iostream>  // std::cout, std::fixed

#include "../../Model/Model.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"

// namespaces
namespace niwa {
namespace asserts {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g., profiling, yields, projections, etc.)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate LaTeX for the documentation.
 */
ObjectiveFunction::ObjectiveFunction(shared_ptr<Model> model) : Assert(model) {
  parameters_.Bind<Double>(PARAM_VALUE, &value_, "Expected value of the objective function", "");
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void ObjectiveFunction::DoBuild() {
  model_->Subscribe(State::kFinalise, this);
}

/**
 * Execute/Run/Process the object.
 */
void ObjectiveFunction::Execute() {
  niwa::ObjectiveFunction& obj = model_->objective_function();
  if (fabs(AS_DOUBLE(value_) - AS_DOUBLE(obj.score())) > (tol_ * fabs(AS_DOUBLE(obj.score())) + tol_)) {
    std::streamsize prec = std::cout.precision();
    std::cout.precision(9);

    LOG_ERROR() << "Assert Failure: Objective Function had actual value " << obj.score() << " when " << value_
                << " values were expected with difference: " << fabs(AS_DOUBLE(value_) - AS_DOUBLE(obj.score()));

    std::cout.precision(prec);
  }
}

} /* namespace asserts */
} /* namespace niwa */
