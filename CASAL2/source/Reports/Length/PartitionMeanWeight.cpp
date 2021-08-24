/**
 * @file PartitionMeanWeight.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 7/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */

// headers
#include "PartitionMeanWeight.h"

#include "../../AgeLengths/Manager.h"
#include "../../Categories/Categories.h"
#include "../../LengthWeights/Manager.h"
#include "../../Model/Model.h"
#include "../../Partition/Accessors/All.h"
#include "../../Partition/Partition.h"
#include "../../TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace reports {
namespace length {

/**
 * Default constructor
 */
PartitionMeanWeight::PartitionMeanWeight() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection | RunMode::kSimulation | RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "The time step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "The years for the report", "", true);
}

/**
 * Build method
 */
void PartitionMeanWeight::DoBuild(shared_ptr<Model> model) {
  if (!parameters_.Get(PARAM_YEARS)->has_been_defined()) {
    years_ = model->years();
  }
}

/**
 * Execute method
 */
void PartitionMeanWeight::DoExecute(shared_ptr<Model> model) {
  unsigned time_step_index = model->managers()->time_step()->current_time_step();

  //  auto categories = Categories::Instance();
  niwa::partition::accessors::All all_view(model);
  vector<Double>                  length_bins = model->length_bins();
  unsigned                        year        = model->current_year();

  cache_ << ReportHeader(type_, label_);
  cache_ << "year: " << year << REPORT_EOL;
  for (auto iterator : all_view) {
    string category = iterator->name_;
    cache_ << category << " " << REPORT_R_LIST << REPORT_EOL;

    cache_ << "mean_weights " << REPORT_R_LIST << REPORT_EOL;
    cache_ << "values: ";

    for (unsigned length_bin_index = 0; length_bin_index <= length_bins.size(); ++length_bin_index)
      cache_ << iterator->mean_weight_by_time_step_length_[time_step_index][length_bin_index] << " ";
    cache_ << REPORT_EOL;

    cache_ << REPORT_R_LIST_END << REPORT_EOL;

    cache_ << REPORT_R_LIST_END << REPORT_EOL;
  }

  ready_for_writing_ = true;
}

} /* namespace length */
} /* namespace reports */
} /* namespace niwa */
