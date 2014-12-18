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

namespace niwa {
namespace reports {

PartitionBiomass::PartitionBiomass() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "Time Step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years", "", true);
  parameters_.Bind<string>(PARAM_UNITS, &units_, "Units (Default Kgs)", "", PARAM_KGS);
}

/**
 *
 */
void PartitionBiomass::DoValidate() {
  ModelPtr model = Model::Instance();
  vector<unsigned> model_years = model->years();
  for (unsigned year : years_) {
    if (std::find(model_years.begin(), model_years.end(), year) == model_years.end())
      LOG_ERROR(parameters_.location(PARAM_YEARS) << " value " << year << " is not a valid year in the model");
  }

  if (units_ != PARAM_TONNES && units_ != PARAM_KGS && units_ != PARAM_GRAMS)
    LOG_ERROR(parameters_.location(PARAM_UNITS) << " value " << units_ << " is not valid. Valid units are tonnes, kgs and grams");
}

/**
 *
 */
void PartitionBiomass::DoExecute() {
  // First, figure out the lowest and highest ages/length
  unsigned lowest         = 9999;
  unsigned highest        = 0;
  unsigned longest_length = 0;

  niwa::partition::accessors::All all_view;
  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    if (lowest > (*iterator)->min_age_)
      lowest = (*iterator)->min_age_;
    if (highest < (*iterator)->max_age_)
      highest = (*iterator)->max_age_;
    if (longest_length < (*iterator)->name_.length())
      longest_length = (*iterator)->name_.length();
  }

  // Print the header
  cache_ << "*" << this->label() << "\n";
  cache_ << "report.type: partition_biomass\n";
  cache_ << "year: " << Model::Instance()->current_year() << "\n";
  cache_ << "time_step: " << time_step_ << "\n";
  cache_ << "category";
  for (unsigned i = lowest; i <= highest; ++i)
    cache_ << " " << i;
  cache_ << "\n";

  cache_.precision(1);
  cache_ << std::fixed;

  unsigned year = Model::Instance()->current_year();

  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    cache_ << (*iterator)->name_;
    for (unsigned i = 0; i < (*iterator)->data_.size(); ++i) {
      unsigned age = (*iterator)->min_age_ + i;
      if (age >= lowest && age <= highest)
        cache_ << " " << std::fixed << std::setprecision(6) << AS_DOUBLE(((*iterator)->data_[i] * (*iterator)->age_size_weight_->mean_weight(year, age)));
      else
        cache_ << " " << "null";
    }
    cache_ << "\n";
  }
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
