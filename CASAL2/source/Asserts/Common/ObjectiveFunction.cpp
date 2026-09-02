/**
 * @file ObjectiveFunction.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 1/09/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "ObjectiveFunction.h"

#include <iomanip>   // std::setprecision
#include <iostream>  // std::cout, std::fixed

#include "../../Model/Model.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "Utilities/String.h"

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
  parameters_.Bind<Double>(PARAM_VALUE, &values_, "Expected value(s) of the objective function", "");
}

/**
 * Build any objects that will need to be utilised by this object.
 * Obtain smart_pointers to any objects that will be used by this object.
 */
void ObjectiveFunction::DoBuild() {
  model()->Subscribe(State::kFinalise, this);
}

/**
 * Execute/Run/Process the object.
 */
void ObjectiveFunction::Execute() {
  niwa::ObjectiveFunction& obj  = model()->objective_function();
  std::streamsize          prec = std::cout.precision();
  std::cout.precision(12);

  bool assert_passed = false;
  for (auto& value_ : values_) {
    if (utilities::math::IsBasicallyEqual(AS_DOUBLE(value_), AS_DOUBLE(obj.score()), tol_)) {
      LOG_INFO() << "Assert Passed: The Assert with label '" << label_ << "' for the Objective Function had value " << obj.score() << " and " << value_ << " was expected.";
      assert_passed = true;
      break;
    }
  }

  if (!assert_passed) {
    string values_str = utilities::String::join(values_, ", ");
    if (error_type_ == PARAM_ERROR) {
      LOG_ERROR() << "Assert Failure: The Assert with label '" << label_ << "' for the Objective Function had value " << obj.score() << " and none of the expected values were matched. The expected values were: " << values_str;
    } else {
      LOG_WARNING() << "Assert Failure: The Assert with label '" << label_ << "' for the Objective Function had value " << obj.score() << " and none of the expected values were matched. The expected values were: " << values_str;      
    }
  }

  std::cout.precision(prec);
}

} /* namespace asserts */
} /* namespace niwa */
