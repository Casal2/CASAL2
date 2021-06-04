/**
 * @file InitialisationPartition.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 25/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "InitialisationPartition.h"

#include <iomanip>
#include <iostream>

#include "../../Model/Model.h"
#include "../../Partition/Accessors/All.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 *
 * Set the run mode and model state for this report
 */
InitialisationPartition::InitialisationPartition() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kInitialise;
}

/**
 * Execute the report
 */
void InitialisationPartition::DoExecute(shared_ptr<Model> model) {
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
  LOG_FINEST() << "min age = " << lowest << ", max age = " << highest << ", longest_length = " << longest_length;

  // Print the header
  cache_ << "*" << type_ << "[" << label_ << "]"
         << "\n";
  cache_ << "values " << REPORT_R_DATAFRAME << "\n";
  cache_ << "category";
  for (unsigned i = lowest; i <= highest; ++i) cache_ << " " << i;
  cache_ << "\n";

  for (auto iterator : all_view) {
    cache_ << iterator->name_;
    unsigned age = iterator->min_age_;
    for (auto value : iterator->data_) {
      if (age >= lowest && age <= highest) {
        //        Double value = *value;
        cache_ << " " << std::fixed << AS_DOUBLE(value);
      } else
        cache_ << " "
               << "null";
      ++age;
    }
    cache_ << "\n";
  }
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
