/*
 * GrowthIncrement.cpp
 *
 *  Created on: 27/02/2018
 *      Author: Zaita
 */
#include "GrowthIncrement.h"

#include "../../GrowthIncrements/GrowthIncrement.h"
#include "../../GrowthIncrements/Manager.h"
#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Partition/Category.h"
#include "../../Partition/Partition.h"

namespace niwa {
namespace reports {
namespace length {

GrowthIncrement::GrowthIncrement() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kExecute;
  skip_tags_   = true;
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "The time step label", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for the report", "", true);
  parameters_.Bind<string>(PARAM_GROWTH_INCREMENT, &growth_label_, "The growth increment label", "");
}
/**
 * DoValidate get pointer
 */
void GrowthIncrement::DoValidate(shared_ptr<Model> model) {
  if (!parameters_.Get(PARAM_YEARS)->has_been_defined()) {
    years_ = model->years();
  }
}
/**
 * DoBuild get pointer
 */
void GrowthIncrement::DoBuild(shared_ptr<Model> model) {}
/**
 * Execute the report
 */
void GrowthIncrement::DoExecute(shared_ptr<Model> model) {
  LOG_FINEST() << "";
  growth_increment_ = model->managers()->growth_increment()->GetGrowthIncrement(growth_label_);
  if (!growth_increment_)
    LOG_FATAL() << "Could not find growth_increment " << growth_label_ << " for the report";
  // Print the header
  if (!growth_increment_)
    LOG_CODE_ERROR() << "Could not find growth_increment_ pointer, this should be addressed in DoBuild()";

  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "year: " << model->current_year() << REPORT_EOL;
  cache_ << "time_step: " << time_step_ << REPORT_EOL;
  growth_increment_->FillReportCache(cache_);
  cache_ << REPORT_END << REPORT_EOL;
  ready_for_writing_ = true;
}

void GrowthIncrement::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_AGE_LENGTH << " has not been implemented";
}

}  // namespace age
} /* namespace reports */
} /* namespace niwa */
