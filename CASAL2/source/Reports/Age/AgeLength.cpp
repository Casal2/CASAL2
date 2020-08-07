/*
 * AgeLength.cpp
 *
 *  Created on: 27/02/2018
 *      Author: Zaita
 */
#include "AgeLength.h"

#include "Model/Managers.h"
#include "Model/Model.h"
#include "AgeLengths/Manager.h"
#include "AgeLengths/AgeLength.h"
#include "Partition/Category.h"
#include "Partition/Partition.h"

namespace niwa {
namespace reports {
namespace age {

/**
 * Default constructor
 */
AgeLength::AgeLength(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "The time step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for the report", "", true);
  parameters_.Bind<string>(PARAM_AGE_LENGTH, &age_length_label_, "The age-length label", "");
  parameters_.Bind<string>(PARAM_CATEGORY, &category_, "The category label", "");
}

/**
 * Validate method
 */
void AgeLength::DoValidate() {
  if (model_->length_bins().size() == 0) {
    LOG_ERROR() << "There are no model length bins defined, so 'age_length' report " << label_ << " cannot be used.";
  }

 if (!parameters_.Get(PARAM_YEARS)->has_been_defined()) {
   years_ = model_->years();
 }
}

/**
 * Execute the report
 */
void AgeLength::DoExecute() {
  auto age_length = model_->managers().age_length()->FindAgeLength(age_length_label_);
  if (!age_length)
    LOG_FATAL() << "Could not find age_length " << age_length_label_ << " for the report";

  unsigned min_age = model_->min_age();
  unsigned max_age = model_->max_age();
  unsigned year = model_->current_year();
  unsigned time_steps = model_->time_steps().size();

  // Print the header
  cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
  cache_ << "year: " << model_->current_year() << "\n";
  cache_ << "time_step: " << time_step_ << "\n";

  cache_ << "year time_step ";
  for (unsigned age = min_age; age <= max_age; ++age)
    cache_ << age << " ";
  cache_ << "\n";

  for (unsigned time_step = 0; time_step < time_steps; ++time_step) {
    cache_ << year << " " << time_step << " ";
    for (unsigned age = min_age; age <= max_age; ++age)
      cache_ << AS_VALUE(age_length->cv(year, time_step, age)) << " ";
    cache_ << "\n";
  }

  cache_ << "\n\n";

  cache_ << "year time_step ";
  cache_ << "age  ";
  for (auto length : model_->length_bins())
    cache_ << "L(" << length << ") ";
  cache_ << "\n";

  unsigned start_year = model_->start_year();
  auto age_lengths = model_->partition().age_length_proportions(category_); // map<category, vector<year, time_step, age, length, proportion>>;
  for (unsigned i = 0; i < age_lengths.size(); ++i) {
    if (std::find(years_.begin(), years_.end(), i + start_year) != years_.end()) {
      for (unsigned j = 0; j < age_lengths[i].size(); ++j) {
        for (unsigned k = 0; k < age_lengths[i][j].size(); ++k) {
          cache_ << (start_year + i) << " " << j << " " << (min_age + k) << " ";
          for (unsigned l = 0; l < age_lengths[i][j][k].size(); ++l) {
            cache_ << AS_VALUE(age_lengths[i][j][k][l]) << " ";
          }
          cache_ << "\n";
        }
      }
    }
  }

  ready_for_writing_ = true;
}

} /* namespaec age */
} /* namespace reports */
} /* namespace niwa */
