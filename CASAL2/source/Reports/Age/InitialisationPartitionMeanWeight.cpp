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

#include <iomanip>
#include <iostream>

#include "../../Model/Model.h"
#include "../../Partition/Accessors/All.h"
#include "../../TimeSteps/Manager.h"
#include "AgeLengths/AgeLength.h"

// namespaces
namespace niwa {
namespace reports {
namespace age {

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
 * Execute the report
 */
void InitialisationPartitionMeanWeight::DoExecute(shared_ptr<Model> model) {
  LOG_TRACE();
  //  auto categories = Categories::Instance();
  niwa::partition::accessors::All all_view(model);
  unsigned                        time_step_index = model->managers()->time_step()->current_time_step();

  cache_ << ReportHeader(type_, label_, format_);
  for (auto iterator : all_view) {
    string category = iterator->name_;
    cache_ << category << " " << REPORT_R_LIST << REPORT_EOL;

    cache_ << "mean_weights " << REPORT_R_LIST << REPORT_EOL;
    cache_ << "values: ";

    for (unsigned age = iterator->min_age_; age <= iterator->max_age_; ++age) cache_ << (iterator->age_length_->mean_weight(time_step_index, age)) << " ";
    cache_ << REPORT_EOL;

    cache_ << REPORT_R_LIST_END << REPORT_EOL;

    cache_ << "age_lengths " << REPORT_R_LIST << REPORT_EOL;
    cache_ << "values: ";

    unsigned age_bins = iterator->age_spread();
    for (unsigned age_index = 0; age_index < age_bins; ++age_index) cache_ << (iterator->age_length_->mean_length(time_step_index, age_index)) << " ";
    cache_ << REPORT_EOL;
    cache_ << REPORT_R_LIST_END << REPORT_EOL;
    cache_ << REPORT_R_LIST_END << REPORT_EOL;
  }
  ready_for_writing_ = true;
}

void InitialisationPartitionMeanWeight::DoExecuteTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_INITIALISATION_PARTITION_MEAN_WEIGHT << " has not been implemented";
}

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */
