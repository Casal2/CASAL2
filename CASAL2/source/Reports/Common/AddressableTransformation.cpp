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
  parameters_.Bind<string>(PARAM_PARAMETER_TRANSFORMATION, &addressable_label_, "The parameter_transformation label that is reported", "","");
}

/**
 * Build the transformation report
 */
void AddressableTransformation::DoBuild(shared_ptr<Model> model) {
  if(addressable_label_ == "") {
    print_all_transformations_ = true;
    LOG_FINE() << "user did not supplied label";
  } else {
    print_all_transformations_ = false;
    LOG_FINE() << "user did not supplied label " << addressable_label_;

  }
  if(!print_all_transformations_) {
    transformation_ = model->managers()->addressable_transformation()->GetAddressableTransformation(addressable_label_);
    if(!transformation_)
      LOG_ERROR_P(PARAM_PARAMETER_TRANSFORMATION) << " Could not find @" << PARAM_PARAMETER_TRANSFORMATION << " labelled " << addressable_label_;
  }
}

/**
 * Execute the estimate summary report
 */
void AddressableTransformation::DoExecute(shared_ptr<Model> model) {
  LOG_FINE();
  // Print the EstimableTransformation
  if(print_all_transformations_) {
    vector<niwa::AddressableTransformation*> addressable_transformations = model->managers()->addressable_transformation()->objects();
    if (addressable_transformations.size() > 0) {
      cache_ << ReportHeader(type_, label_, format_);
      for (auto addressable_transform : addressable_transformations) {
        cache_ << addressable_transform->label() << " " << REPORT_R_LIST << REPORT_EOL;
        addressable_transform->FillReportCache(cache_);
        cache_ << REPORT_R_LIST_END << REPORT_EOL;
      }
      ready_for_writing_ = true;
    }
  } else {
    if(!print_all_transformations_)
      transformation_ = model->managers()->addressable_transformation()->GetAddressableTransformation(addressable_label_);
    if(!transformation_)
      LOG_CODE_ERROR() << "(!transformation_): " << addressable_label_;
    cache_ << ReportHeader(type_, label_, format_);
    transformation_->FillReportCache(cache_);
    ready_for_writing_ = true;
  }
}

void AddressableTransformation::DoPrepareTabular(shared_ptr<Model> model) {
  if(print_all_transformations_)
    LOG_ERROR_P(PARAM_LABEL) << "This report needs a specified " << PARAM_PARAMETER_TRANSFORMATION << " for tabular mode";

}

void AddressableTransformation::DoExecuteTabular(shared_ptr<Model> model) {
  if(!print_all_transformations_) {
    transformation_ = model->managers()->addressable_transformation()->GetAddressableTransformation(addressable_label_);
    if(!transformation_)
      LOG_CODE_ERROR() << "(!transformation_): " << addressable_label_;
  }
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
