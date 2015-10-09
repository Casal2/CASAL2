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

#include <iostream>
#include <iomanip>

#include "Model/Model.h"
#include "Partition/Accessors/All.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor.
 *
 * Set the run mode and model state for this report
 */
InitialisationPartition::InitialisationPartition(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kInitialise;
}

/**
 *
 */
void InitialisationPartition::DoExecute() {
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
  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
  cache_ << "year: " << Model::Instance()->current_year() << "\n";
  cache_ << "time_step: " << time_step_ << "\n";
  cache_ << "values "<< REPORT_R_DATAFRAME<<"\n";
  cache_ << "category";
  for (unsigned i = lowest; i <= highest; ++i)
    cache_ << " " << i;
  cache_ << "\n";

  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    cache_ << (*iterator)->name_;
    unsigned age = (*iterator)->min_age_;
    for (auto values = (*iterator)->data_.begin(); values != (*iterator)->data_.end(); ++values, age++) {
      if (age >= lowest && age <= highest)
        cache_ << " " << std::fixed << *values;
      else
        cache_ << " " << "null";
    }
    cache_ << "\n";
  }
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
