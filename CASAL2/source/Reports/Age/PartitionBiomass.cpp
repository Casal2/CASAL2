/*
 * PartitionBiomass.cpp
 *
 *  Created on: 18/12/2014
 *      Author: Admin
 */

#include "PartitionBiomass.h"

#include <iomanip>
#include <iostream>

#include "../../Model/Model.h"
#include "../../Partition/Accessors/All.h"
#include "../../TimeSteps/Manager.h"

namespace niwa {
namespace reports {
namespace age {

PartitionBiomass::PartitionBiomass() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "The time step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for the report", "", true);
}

/**
 *
 */
void PartitionBiomass::DoValidate(shared_ptr<Model> model) {
  vector<unsigned> model_years = model->years();
  for (unsigned year : years_) {
    if (std::find(model_years.begin(), model_years.end(), year) == model_years.end())
      LOG_ERROR_P(PARAM_YEARS) << " value " << year << " is not a valid year in the model";
  }
}

/**
 *
 */
void PartitionBiomass::DoExecute(shared_ptr<Model> model) {
  // First, figure out the lowest and highest ages/length
  unsigned lowest          = 9999;
  unsigned highest         = 0;
  unsigned longest_length  = 0;
  unsigned time_step_index = model->managers()->time_step()->current_time_step();

  niwa::partition::accessors::All all_view(model);
  for (auto iterator : all_view) {
    if (lowest > iterator->min_age_)
      lowest = iterator->min_age_;
    if (highest < iterator->max_age_)
      highest = iterator->max_age_;
    if (longest_length < iterator->name_.length())
      longest_length = iterator->name_.length();
  }
  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "year: " << model->current_year() << REPORT_EOL;
  cache_ << "time_step: " << time_step_ << REPORT_EOL;
  cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;

  cache_ << "category";
  for (unsigned i = lowest; i <= highest; ++i) cache_ << " " << i;
  cache_ << REPORT_EOL;

  cache_.precision(1);  // TODO: Validate why this is here?
  cache_ << std::fixed;

  for (auto iterator : all_view) {
    cache_ << iterator->name_;
    for (unsigned i = 0; i < iterator->data_.size(); ++i) {
      unsigned age = iterator->min_age_ + i;
      if (age >= lowest && age <= highest)
        cache_ << " " << std::fixed << std::setprecision(5) << AS_DOUBLE((iterator->data_[i] * iterator->mean_weight_by_time_step_age_[time_step_index][age]));
      else
        cache_ << " "
               << "null";
    }
    cache_ << REPORT_EOL;
  }

  ready_for_writing_ = true;
}

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */
