/**
 * @file Addressable.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 20/06/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "Addressable.h"

#include <iomanip>

#include "../../Estimates/Common/Uniform.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"
#include "../../Utilities/To.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
Addressable::Addressable() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProjection | RunMode::kProfiling);
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_PARAMETER, &parameter_, "The addressable parameter name", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Define the years that the report is generated for", "");
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "Defines the time-step that the report applies to", "", "");
}

/**
 *
 */
void Addressable::DoBuild(shared_ptr<Model> model) {
  string type      = "";
  string label     = "";
  string parameter = "";
  string index     = "";

  /**
   * Explode the parameter string so we can get the addressable
   * name (parameter) and the index
   */
  if (parameter_ == "") {
    parameters().Add(PARAM_PARAMETER, label_, parameters_.Get(PARAM_LABEL)->file_name(), parameters_.Get(PARAM_LABEL)->line_number());
    parameter_ = label_;
  }

  string error = "";
  if (!model->objects().VerifyAddressableForUse(parameter_, addressable::kLookup, error)) {
    LOG_FATAL_P(PARAM_PARAMETER) << "could not be verified for use in assert.addressable. Error: " << error;
  }
}

/**
 * Prepare the report
 */
void Addressable::DoPrepare(shared_ptr<Model> model) {
  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "years: ";
  for (unsigned year : years_) cache_ << std::left << std::setw(10) << year;
  cache_ << REPORT_EOL;
  cache_ << "value: ";
}

/**
 * Execute the report
 */
void Addressable::DoExecute(shared_ptr<Model> model) {
  LOG_TRACE();
  target_ = model->objects().GetAddressable(parameter_);
  if (target_ == nullptr)
    LOG_CODE_ERROR() << "(target_ == nullptr)";
  cache_ << std::left << std::setw(10) << *target_;
}

/**
 * Finalise the report
 */
void Addressable::DoFinalise(shared_ptr<Model> model) {
  cache_ << REPORT_EOL;
  ready_for_writing_ = true;
}
void Addressable::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_PARAMETER_TRANSFORMATIONS << " has not been implemented";
}

void Addressable::DoExecuteTabular(shared_ptr<Model> model) {

}

void Addressable::DoFinaliseTabular(shared_ptr<Model> model) {

}

} /* namespace reports */
} /* namespace niwa */
