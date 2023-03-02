/*
 * AgeLength.cpp
 *
 *  Created on: 27/02/2018
 *      Author: Zaita
 */
#include "AgeLength.h"

#include "../../AgeLengths/AgeLength.h"
#include "../../AgeLengths/Manager.h"
#include "../../Model/Managers.h"
#include "../../Model/Model.h"
#include "../../Partition/Category.h"
#include "../../Partition/Partition.h"

namespace niwa {
namespace reports {
namespace age {

AgeLength::AgeLength() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kExecute;
  skip_tags_   = true;
  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "The time step label", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for the report", "", true);
  parameters_.Bind<string>(PARAM_AGE_LENGTH, &age_length_label_, "The age-length label", "");
}
/**
 * DoValidate get pointer
 */
void AgeLength::DoValidate(shared_ptr<Model> model) {
  if (!parameters_.Get(PARAM_YEARS)->has_been_defined()) {
    years_ = model->years();
  }
}
/**
 * DoBuild get pointer
 */
void AgeLength::DoBuild(shared_ptr<Model> model) {}
/**
 * Execute the report
 */
void AgeLength::DoExecute(shared_ptr<Model> model) {
  LOG_FINEST() << "";
  age_length_ = model->managers()->age_length()->FindAgeLength(age_length_label_);
  if (!age_length_)
    LOG_FATAL() << "Could not find age_length " << age_length_label_ << " for the report";
  // Print the header
  if (!age_length_)
    LOG_CODE_ERROR() << "Could not find age_length pointer, this should be addressed in DoBuild()";

  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "year: " << model->current_year() << REPORT_EOL;
  cache_ << "time_step: " << time_step_ << REPORT_EOL;
  cache_ << "age:";
  for (unsigned age = model->min_age(); age <= model->max_age(); ++age) cache_ << " " << age;
  cache_ << REPORT_EOL;

  age_length_->FillReportCache(cache_);
  cache_ << REPORT_END << REPORT_EOL;
  ready_for_writing_ = true;
}

void AgeLength::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_AGE_LENGTH << " has not been implemented";
}

}  // namespace age
} /* namespace reports */
} /* namespace niwa */
