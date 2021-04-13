/**
 * @file InitialisationPartitionMeanWeight.cpp
 * @author C.Marsh
 * @date17/07/2017
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "InitialisationPartitionMeanWeight.h"

#include <iostream>
#include <iomanip>

#include "../../Model/Model.h"
#include "../../Partition/Accessors/All.h"
#include "../../TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace reports {
namespace length {

/**
 * Default constructor.
 *
 * Set the run mode and model state for this report
 */
InitialisationPartitionMeanWeight::InitialisationPartitionMeanWeight() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kInitialise;
}


/**
 *
 */
void InitialisationPartitionMeanWeight::DoExecute(shared_ptr<Model> model) {
  LOG_TRACE();
  //  auto categories = Categories::Instance();
  niwa::partition::accessors::All all_view(model);
  unsigned time_step_index = model->managers()->time_step()->current_time_step();
  vector<Double> length_bins = model->length_bins();
  cache_ << "*" << type_ << "[" << label_ << "]\n";
  for (auto iterator : all_view) {

    string category = iterator->name_;
    cache_ << category << " " << REPORT_R_LIST << "\n";

    cache_ << "mean_weights " << REPORT_R_LIST << "\n";
    cache_ << "values: ";

    for (unsigned length_bin_index = 0; length_bin_index <= length_bins.size(); ++length_bin_index)
      cache_ << AS_DOUBLE(iterator->mean_weight_by_time_step_length_[time_step_index][length_bin_index]) << " ";
    cache_<<"\n";

    cache_ << REPORT_R_LIST_END <<"\n";

    cache_ << REPORT_R_LIST_END <<"\n";
  }

  ready_for_writing_ = true;
}

} /* namespace length */
} /* namespace reports */
} /* namespace niwa */
