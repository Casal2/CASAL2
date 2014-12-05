/**
 * @file Estimable.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 20/06/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Estimable.h"

#include <iomanip>

#include "Estimates/Children/Uniform.h"
#include "ObjectsFinder/ObjectsFinder.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
Estimable::Estimable() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProjection | RunMode::kProfiling);
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "Parameter to print", "");
}

void Estimable::DoValidate() {

}

void Estimable::DoBuild() {
  string type       = "";
  string label      = "";
  string parameter  = "";
  string index      = "";

  /**
   * Explode the parameter string so we can get the estimable
   * name (parameter) and the index
   */
  if (parameter_ == "") {
    parameters().Add(PARAM_PARAMETER, label_, parameters_.Get(PARAM_LABEL)->file_name(), parameters_.Get(PARAM_LABEL)->line_number());
    parameter_ = label_;
  }

  objects::ExplodeString(parameter_, type, label, parameter, index);
  if (type == "" || label == "" || parameter == "") {
    LOG_ERROR(parameters_.location(PARAM_PARAMETER) << ": parameter " << parameter_
        << " is not in the correct format. Correct format is object_type[label].estimable(array index)");
  }
  objects::ImplodeString(type, label, parameter, index, parameter_);

  base::ObjectPtr target = objects::FindObject(parameter_);
  if (!target) {
    LOG_ERROR(parameters_.location(PARAM_PARAMETER) << ": parameter " << parameter_ << " is not a valid estimable in the system");
  }

  if (index != "")
    target_ = target->GetEstimable(parameter, index);
  else
    target_ = target->GetEstimable(parameter);

  if (target_ == 0)
    LOG_CODE_ERROR("if (target_ == 0)");
}

/**
 *
 */
void Estimable::DoPrepare() {
  cache_ << "*" << this->label() << "\n";
  cache_ << "years: ";
  for (unsigned year : years_)
    cache_ << std::left << std::setw(10) << year;
  cache_ << "\n";
  cache_ << "value: ";
}

/**
 *
 */
void Estimable::DoExecute() {
  LOG_TRACE();
  cache_ << std::left << std::setw(10) << *target_;
}

/**
 *
 */
void Estimable::DoFinalise() {
  cache_ << "\n";
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
