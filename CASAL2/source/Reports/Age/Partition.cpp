/**
 * @file Partition.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Partition.h"

#include <iomanip>
#include <iostream>

#include "../../Model/Model.h"
#include "../../Partition/Accessors/All.h"

// Namespaces
namespace niwa {
namespace reports {
namespace age {

/**
 * Default constructor
 */
Partition::Partition() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kExecute;
  skip_tags_ = true;

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "Time Step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years", "", true);
}

void Partition::DoValidate(shared_ptr<Model> model) {
  if (!parameters_.Get(PARAM_YEARS)->has_been_defined()) {
    years_ = model->years();
  }
}
/**
 * Execute the report
 */
void Partition::DoExecute(shared_ptr<Model> model) {
  LOG_TRACE();
  // First, figure out the lowest and highest ages/length
  unsigned lowest         = 9999;
  unsigned highest        = 0;
  unsigned longest_length = 0;

  niwa::partition::accessors::All all_view(model);
  for (auto iterator : all_view) {
    if (lowest > iterator->min_age_)
      lowest = iterator->min_age_;
    if (highest < iterator->max_age_)
      highest = iterator->max_age_;
    if (longest_length < iterator->name_.length())
      longest_length = iterator->name_.length();
  }

  // Print the header
  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "year: " << model->current_year() << REPORT_EOL;
  cache_ << "time_step: " << time_step_ << REPORT_EOL;
  cache_ << "values " << REPORT_R_DATAFRAME_ROW_LABELS << REPORT_EOL;
  cache_ << "category";
  for (unsigned i = lowest; i <= highest; ++i) cache_ << " " << i;
  cache_ << REPORT_EOL;

  for (auto iterator : all_view) {
    cache_ << iterator->name_;
    unsigned age = iterator->min_age_;
    for (auto value : iterator->data_) {
      if (age >= lowest && age <= highest) {
        cache_ << " " << std::fixed << AS_DOUBLE(value);
      } else
        cache_ << " "
               << "null";

      ++age;
    }
    cache_ << REPORT_EOL;
  }
  cache_ << REPORT_END << REPORT_EOL;

  ready_for_writing_ = true;
}

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */
