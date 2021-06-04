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

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "The time step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for the report", "", true);
  parameters_.Bind<string>(PARAM_AGE_LENGTH, &age_length_label_, "The age-length label", "");
  parameters_.Bind<string>(PARAM_CATEGORY, &category_, "The category label", "");
}

void AgeLength::DoValidate(shared_ptr<Model> model) {
  if (!parameters_.Get(PARAM_YEARS)->has_been_defined()) {
    years_ = model->years();
  }
}

/**
 * Execute the report
 */
void AgeLength::DoExecute(shared_ptr<Model> model) {
  auto age_length = model->managers()->age_length()->FindAgeLength(age_length_label_);
  if (!age_length)
    LOG_FATAL() << "Could not find age_length " << age_length_label_ << " for the report";

  unsigned min_age    = model->min_age();
  unsigned max_age    = model->max_age();
  unsigned year       = model->current_year();
  unsigned time_steps = model->time_steps().size();

  // Print the header
  cache_ << "*" << type_ << "[" << label_ << "]"
         << "\n";
  cache_ << "year: " << model->current_year() << "\n";
  cache_ << "time_step: " << time_step_ << "\n";

  cache_ << "year time_step ";
  for (unsigned age = min_age; age <= max_age; ++age) cache_ << age << " ";
  cache_ << "\n";

  for (unsigned time_step = 0; time_step < time_steps; ++time_step) {
    cache_ << year << " " << time_step << " ";
    for (unsigned age = min_age; age <= max_age; ++age) cache_ << AS_DOUBLE(age_length->cv(year, time_step, age)) << " ";
    cache_ << "\n";
  }

  cache_ << "\n\n";

  cache_ << "year time_step ";
  cache_ << "age  ";
  for (auto length : model->length_bins()) cache_ << "L(" << length << ") ";
  cache_ << "\n";

  unsigned start_year  = model->start_year();
  auto     age_lengths = model->partition().age_length_proportions(category_);  // map<category, vector<year, time_step, age, length, proportion>>;
  for (unsigned i = 0; i < age_lengths.size(); ++i) {
    if (std::find(years_.begin(), years_.end(), i + start_year) != years_.end()) {
      for (unsigned j = 0; j < age_lengths[i].size(); ++j) {
        for (unsigned k = 0; k < age_lengths[i][j].size(); ++k) {
          cache_ << (start_year + i) << " " << j << " " << (min_age + k) << " ";
          for (unsigned l = 0; l < age_lengths[i][j][k].size(); ++l) {
            cache_ << AS_DOUBLE(age_lengths[i][j][k][l]) << " ";
          }
          cache_ << "\n";
        }
      }
    }
  }

  ready_for_writing_ = true;
}

}  // namespace age
} /* namespace reports */
} /* namespace niwa */
