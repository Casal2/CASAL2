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
#include "../../Model/Model.h"
#include "../../Model/Objects.h"
#include "../../Utilities/To.h"

// namespaces
namespace niwa {
namespace reports {
/**
 * Default Constructor
 */
AddressableTransformation::AddressableTransformation() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProfiling | RunMode::kProjection);
  model_state_ = State::kIterationComplete;
}

/**
 * Execute the estimate summary report
 */
void AddressableTransformation::DoExecute(shared_ptr<Model> model) {
  LOG_FINE();
  // Print the EstimableTransformation
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
}

void AddressableTransformation::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_PARAMETER_TRANSFORMATIONS << " has not been implemented";
}

void AddressableTransformation::DoExecuteTabular(shared_ptr<Model> model) {

}

void AddressableTransformation::DoFinaliseTabular(shared_ptr<Model> model) {

}
} /* namespace reports */
} /* namespace niwa */
