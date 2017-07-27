/**
 * @file Addressable.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 20/06/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include <Common/Reports/Children/Addressable.h>

#include <iomanip>

#include "Common/Estimates/Children/Uniform.h"
#include "Common/Model/Model.h"
#include "Common/Model/Objects.h"
#include "Common/Utilities/To.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
Addressable::Addressable(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProjection | RunMode::kProfiling);
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "Parameter to print", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years to print the addressable for", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "Time Step label", "", "");
}

/**
 *
 */
void Addressable::DoValidate() {

}

/**
 *
 */
void Addressable::DoBuild() {
  string type       = "";
  string label      = "";
  string parameter  = "";
  string index      = "";

  /**
   * Explode the parameter string so we can get the addressable
   * name (parameter) and the index
   */
  if (parameter_ == "") {
    parameters().Add(PARAM_PARAMETER, label_, parameters_.Get(PARAM_LABEL)->file_name(), parameters_.Get(PARAM_LABEL)->line_number());
    parameter_ = label_;
  }

  string error = "";
  if (!model_->objects().VerfiyAddressableForUse(parameter_, addressable::kLookup, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be verified for use in assert.addressable. Error was " << error;
  }
  target_ = model_->objects().GetAddressable(parameter_);
}

/**
 *
 */
void Addressable::DoPrepare() {
  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";

  cache_ << "years: ";
  for (unsigned year : years_)
    cache_ << std::left << std::setw(10) << year;
  cache_ << "\n";
  cache_ << "value: ";
}

/**
 *
 */
void Addressable::DoExecute() {
  LOG_TRACE();
  cache_ << std::left << std::setw(10) << *target_;
}

/**
 *
 */
void Addressable::DoFinalise() {
  cache_ << "\n";
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
