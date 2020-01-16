/**
 * @file UserDefined.cpp
 * @author  C.Marsh
 * @date 5/6/17
 * @section LICENSE
 *
 * Copyright NIWA Science �2017 - www.niwa.co.nz
 *
 */

// headers
#include <boost/algorithm/string/join.hpp>
#include <Projects/Common/UserDefined.h>

#include "EquationParser/EquationParser.h"
#include "Model/Model.h"
#include "Model/Objects.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Default constructor
 */
UserDefined::UserDefined(Model* model) : Project(model) {
  parameters_.Bind<string>(PARAM_EQUATION, &equation_input_, "Equation to do a test run of", "");
}

/**
 * Build
 */
void UserDefined::DoBuild() {
  LOG_TRACE();
  equation_ = boost::algorithm::join(equation_input_, " ");
}

/**
 *
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
  (this->*DoUpdateFunc_)(value_);
}

} /* namespace projects */
} /* namespace niwa */
