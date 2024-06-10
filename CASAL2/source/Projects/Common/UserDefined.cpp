/**
 * @file UserDefined.cpp
 * @author  C.Marsh
 * @date 5/6/17
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

// headers
#include "UserDefined.h"

#include <boost/algorithm/string/join.hpp>

#include "../../EquationParser/EquationParser.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"
#include "../../Utilities/To.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Default constructor
 */
UserDefined::UserDefined(shared_ptr<Model> model) : Project(model) {
  parameters_.Bind<string>(PARAM_EQUATION, &equation_input_, "The equation to do a test run of", "");
}

/**
 * Build the equation
 */
void UserDefined::DoBuild() {
  LOG_TRACE();
  LOG_WARNING() << "Using @project type UserDefined. This is untested functionality.";
  equation_ = boost::algorithm::join(equation_input_, " ");
}

/**
 * Update the equation
 */
void UserDefined::DoUpdate() {
  LOG_TRACE();
  try {
    value_ = model_->equation_parser().Parse(equation_);
  } catch (std::runtime_error& ex) {
    LOG_FATAL() << "In the projection " << label_ << ", the equation " << equation_ << " could not be parsed because: " << ex.what();
  } catch (...) {
    LOG_FATAL() << "result: equation failed\n";
  }

  LOG_FINE() << "Setting Value to: " << value_ << ", in year = " << model_->current_year();
  (this->*DoUpdateFunc_)(value_, true, model_->current_year());
}

} /* namespace projects */
} /* namespace niwa */
