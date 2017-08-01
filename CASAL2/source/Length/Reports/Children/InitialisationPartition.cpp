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

#include "Common/Model/Model.h"
#include "Common/Partition/Accessors/All.h"

// namespaces
namespace niwa {
namespace length {
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
  LOG_TRACE();
  unsigned longest_length = 0;

  niwa::partition::accessors::All all_view(model_);
  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {

    if (longest_length < (*iterator)->name_.length())
      longest_length = (*iterator)->name_.length();
  }

  // Print the header
  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
  cache_ << "year: " << model_->current_year() << "\n";
  cache_ << "time_step: " << time_step_ << "\n";
  cache_ << "values "<< REPORT_R_DATAFRAME<<"\n";
  cache_ << "category";
  for (unsigned i : model_->length_bins())
    cache_ << " " << i;
  cache_ << "\n";

  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    cache_ << (*iterator)->name_;
    for (auto values = (*iterator)->length_data_.begin(); values != (*iterator)->length_data_.end(); ++values) {
        Double value = *values;
        cache_ << " " << std::fixed << AS_DOUBLE(value);
    }
    cache_ << "\n";
  }
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace age */
} /* namespace niwa */
