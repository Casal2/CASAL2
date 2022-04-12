/**
 * @file AddressableTransformation.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 20/06/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */

// headers
#include "AddressableTransformation.h"

#include <iomanip>

#include "../../AddressableTransformations/Manager.h"
#include "../../AddressableTransformations/AddressableTransformation.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"
#include "../../Utilities/To.h"
#include "../../Model/Model.h"
// namespaces
namespace niwa {
namespace reports {
/**
 * Default Constructor
 */
AddressableTransformation::AddressableTransformation() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProfiling | RunMode::kProjection);
  model_state_ = State::kIterationComplete;
  parameters_.Bind<string>(PARAM_PARAMETER_TRANSFORMATION, &addressable_label_, "The parameter_transformation label that is reported", "");
}

/**
 * Build the transformation report
 */
void AddressableTransformation::DoBuild(shared_ptr<Model> model) {
  LOG_FINE() << "here";
  transformation_ = model->managers()->addressable_transformation()->GetAddressableTransformation(addressable_label_);
  if(!transformation_) {
    #ifndef TESTMODE
    //LOG_ERROR_P(PARAM_PARAMETER_TRANSFORMATION) << " Could not find @" << PARAM_PARAMETER_TRANSFORMATION << " labelled " << addressable_label_;
    LOG_WARNING() << "The report for " << PARAM_PARAMETER_TRANSFORMATION << " with label '" << addressable_label_ << "' was requested. This " << PARAM_PARAMETER_TRANSFORMATION
                  << " was not found in the input configuration file and the report will not be generated";
    #endif
    is_valid_ = false;
  }
}

/**
 * Execute the estimate summary report
 */
void AddressableTransformation::DoExecute(shared_ptr<Model> model) {
  LOG_FINE();
  // Print the EstimableTransformation
  if(!print_all_transformations_)
    transformation_ = model->managers()->addressable_transformation()->GetAddressableTransformation(addressable_label_);
  if(!transformation_)
    LOG_CODE_ERROR() << "(!transformation_): " << addressable_label_;
  cache_ << ReportHeader(type_, label_, format_);
  transformation_->FillReportCache(cache_);
  ready_for_writing_ = true;
}

void AddressableTransformation::DoPrepareTabular(shared_ptr<Model> model) {

}

void AddressableTransformation::DoExecuteTabular(shared_ptr<Model> model) {
  LOG_FINE() << "here";
  if (first_run_) {
    first_run_ = false;
    cache_ << ReportHeader(type_, addressable_label_, format_);
    cache_ << "type: " << transformation_->type() << REPORT_EOL;
    cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;
    transformation_->FillTabularReportCache(cache_, true);
  } else {
    transformation_->FillTabularReportCache(cache_, false);
  }
}

void AddressableTransformation::DoFinaliseTabular(shared_ptr<Model> model) {
  ready_for_writing_ = true;
}
} /* namespace reports */
} /* namespace niwa */
