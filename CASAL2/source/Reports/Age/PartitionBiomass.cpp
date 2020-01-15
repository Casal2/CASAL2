/*
 * PartitionBiomass.cpp
 *
 *  Created on: 18/12/2014
 *      Author: Admin
 */

#include "PartitionBiomass.h"

#include <iostream>
#include <iomanip>

#include "Model/Model.h"
#include "Partition/Accessors/All.h"
#include "TimeSteps/Manager.h"

namespace niwa {
namespace reports {
namespace age {

PartitionBiomass::PartitionBiomass(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "Time Step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years", "", true);
}

/**
 *
 */
void PartitionBiomass::DoValidate() {
  vector<unsigned> model_years = model_->years();
  for (unsigned year : years_) {
    if (std::find(model_years.begin(), model_years.end(), year) == model_years.end())
      LOG_ERROR_P(PARAM_YEARS) << " value " << year << " is not a valid year in the model";
  }
}

/**
 *
 */
void PartitionBiomass::DoExecute() {
  // First, figure out the lowest and highest ages/length
  unsigned lowest         = 9999;
  unsigned highest        = 0;
  unsigned longest_length = 0;
  unsigned time_step_index = model_->managers().time_step()->current_time_step();

  niwa::partition::accessors::All all_view(model_);
  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    if (lowest > (*iterator)->min_age_)
      lowest = (*iterator)->min_age_;
    if (highest < (*iterator)->max_age_)
      highest = (*iterator)->max_age_;
    if (longest_length < (*iterator)->name_.length())
      longest_length = (*iterator)->name_.length();
  }
  cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
  cache_ << "year: " << model_->current_year() << "\n";
  cache_ << "time_step: " << time_step_ << "\n";
  cache_ << "values " << REPORT_R_DATAFRAME << "\n";

  cache_ << "category";
  for (unsigned i = lowest; i <= highest; ++i)
    cache_ << " " << i;
  cache_ << "\n";

  cache_.precision(1);
  cache_ << std::fixed;

  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    //(*iterator)->UpdateMeanWeightData();

    cache_ << (*iterator)->name_;
    for (unsigned i = 0; i < (*iterator)->data_.size(); ++i) {
      unsigned age = (*iterator)->min_age_ + i;
      if (age >= lowest && age <= highest)
        cache_ << " " << std::fixed << std::setprecision(5) << AS_DOUBLE(((*iterator)->data_[i] * (*iterator)->mean_weight_by_time_step_age_[time_step_index][age]));
      else
        cache_ << " " << "null";
    }
    cache_ << "\n";
  }

  ready_for_writing_ = true;
}

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */
