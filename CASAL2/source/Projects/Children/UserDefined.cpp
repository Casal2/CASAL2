/**
 * @file UserDefined.cpp
 * @author  C.Marsh
 * @date 5/6/17
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */

// headers
#include "UserDefined.h"

#include <boost/algorithm/string/join.hpp>

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
 * Validate
 */
void UserDefined::DoValidate() {
//  for (unsigned i = 0; i < years_.size(); ++i) {
//    LOG_FINEST() << "value in year " << years_[i] << " = " << values_[i];
//    year_values_[years_[i]] = values_[i];
//  }
}

/**
 * Build
 */
void UserDefined::DoBuild() {
  LOG_TRACE();
  equation_ = boost::algorithm::join(equation_input_, " ");
}

/**
 * Reset
 */
void UserDefined::DoReset() {
  LOG_TRACE();
}

/**
 *
 */
void UserDefined::DoUpdate() {
  LOG_TRACE();
  try {
    value_ = model_->equation_parser().Parse(equation_);
  } catch (std::runtime_error& ex) {
    LOG_FATAL() << "In the projection " << label_ << " we could not parse the following equation " << equation_ << " for the following reason: " << ex.what();
  } catch (...) {
    LOG_FATAL() << "result: equation failed\n";
  }
  LOG_FINE() << "Setting Value to: " << value_ << ", in year = " << model_->current_year();
  year_values_[model_->current_year()] = value_;
  // store for report
  projected_parameters_[model_->current_year()] = value_* multiplier_;

  (this->*DoUpdateFunc_)(value_);
}

} /* namespace projects */
} /* namespace niwa */
