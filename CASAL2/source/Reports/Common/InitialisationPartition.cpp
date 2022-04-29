/**
 * @file InitialisationPartition.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 25/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
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
  skip_tags_   = true;
}

/**
 * Execute the report
 */
void InitialisationPartition::DoExecute(shared_ptr<Model> model) {
  LOG_TRACE();
  // First, figure out the lowest and highest ages/length

  niwa::partition::accessors::All all_view(model);

  // Print the header
  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "values " << REPORT_R_DATAFRAME_ROW_LABELS << REPORT_EOL;
  cache_ << "category";
  if(model->partition_type() == PartitionType::kAge) {
    for (unsigned i = model->min_age(); i <= model->max_age(); ++i) 
      cache_ << " " << i;
    cache_ << REPORT_EOL;
  } else if (model->partition_type() == PartitionType::kLength) {
    for (auto len_bin : model->length_bin_mid_points()) 
      cache_ << " " << len_bin;
    cache_ << REPORT_EOL;
  }

  for (auto iterator : all_view) {
    cache_ << iterator->name_;
    for (auto value : iterator->data_) {
        cache_ << " " << std::fixed << AS_DOUBLE(value);
    }
    cache_ << REPORT_EOL;
  }
  cache_ << REPORT_END << REPORT_EOL;
  ready_for_writing_ = true;
}

void InitialisationPartition::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_INITIALISATION_PARTITION << " has not been implemented";
}

void InitialisationPartition::DoExecuteTabular(shared_ptr<Model> model) {

}

void InitialisationPartition::DoFinaliseTabular(shared_ptr<Model> model) {

}

} /* namespace reports */
} /* namespace niwa */
