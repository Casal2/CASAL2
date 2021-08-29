/**
 * @file Partition_YearCrossAgeMatrix.cpp
 * @author  Marco Kienzle (Marco.Kienzle@niwa.co.nz) based on Partition.cpp by Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 26/08/2018
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: Sunday 26 August  08:19:08 NZST 2018 $
 */

// Headers
#include "PartitionYearCrossAgeMatrix.h"

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
Partition_YearCrossAgeMatrix::Partition_YearCrossAgeMatrix() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "The time step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for the report", "", true);
}

/**
 * Validate
 */
void Partition_YearCrossAgeMatrix::DoValidate(shared_ptr<Model> model) {
  if (!parameters_.Get(PARAM_YEARS)->has_been_defined()) {
    years_ = model->years();
  }
}

/**
 * Prepare the report
 */
void Partition_YearCrossAgeMatrix::DoPrepare(shared_ptr<Model> model) {
  unsigned lowest  = 9999;
  unsigned highest = 0;

  niwa::partition::accessors::All all_view(model);

  for (auto iterator : all_view) {
    if (lowest > iterator->min_age_)
      lowest = iterator->min_age_;
    if (highest < iterator->max_age_)
      highest = iterator->max_age_;
  }

  // Print the header
  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "time_step: " << time_step_ << REPORT_EOL;

  // Print the age-groups
  const char separator = ' ';
  // const int nameWidth = 6;
  const int numWidth = 13;
  cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;
  cache_ << std::left << std::setw(numWidth) << std::setfill(separator) << "year";  // leave an empty space in the years's column
  for (unsigned i = lowest; i <= highest; ++i) cache_ << std::left << std::setw(numWidth) << std::setfill(separator) << "AG" + std::to_string(i);
  cache_ << REPORT_EOL;
}

/**
 * Execute the report
 */
void Partition_YearCrossAgeMatrix::DoExecute(shared_ptr<Model> model) {
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

  const char separator = ' ';
  const int  numWidth  = 13;

  for (auto iterator : all_view) {
    cache_ << std::left << std::setw(numWidth) << std::setfill(separator) << std::setprecision(1) << std::fixed << model->current_year();
    unsigned age = iterator->min_age_;
    for (auto value : iterator->data_) {
      if (age >= lowest && age <= highest) {
        cache_ << std::left << std::setw(numWidth) << std::setfill(separator) << std::setprecision(0) << std::fixed << AS_DOUBLE(value);
      } else
        cache_ << " "
               << "null";
      ++age;
    }
    cache_ << REPORT_EOL;
  }
}

/**
 *
 */
void Partition_YearCrossAgeMatrix::DoFinalise(shared_ptr<Model> model) {
  ready_for_writing_ = true;
}

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */
