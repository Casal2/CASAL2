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
namespace length {

/**
 * Default constructor
 */
PartitionBiomass::PartitionBiomass() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "The time step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for the report", "", true);
}

/**
 * Validate
 */
void PartitionBiomass::DoValidate(shared_ptr<Model> model) {
  vector<unsigned> model_years = model->years();
  for (unsigned year : years_) {
    if (std::find(model_years.begin(), model_years.end(), year) == model_years.end())
      LOG_ERROR_P(PARAM_YEARS) << " value " << year << " is not a valid year in the model";
  }
}

/**
 * Execute the report
 */
void PartitionBiomass::DoExecute(shared_ptr<Model> model) {
  // First, figure out the lowest and highest ages/length
  unsigned       time_step_index = model->managers()->time_step()->current_time_step();
  vector<Double> length_bins     = model->length_bins();

  niwa::partition::accessors::All all_view(model);

  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "year: " << model->current_year() << REPORT_EOL;
  cache_ << "time_step: " << time_step_ << REPORT_EOL;
  cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;

  cache_ << "category";
  for (auto length_bin : length_bins) cache_ << " " << length_bin;
  cache_ << REPORT_EOL;

  cache_.precision(1);
  cache_ << std::fixed;

  for (auto iterator : all_view) {
    cache_ << iterator->name_;
    for (unsigned i = 0; i < iterator->data_.size(); ++i) {
      cache_ << " " << std::fixed << std::setprecision(5) << AS_DOUBLE((iterator->data_[i] * iterator->mean_weight_by_time_step_length_[time_step_index][i]));
    }
    cache_ << REPORT_EOL;
  }

  ready_for_writing_ = true;
}

} /* namespace length */
} /* namespace reports */
} /* namespace niwa */
