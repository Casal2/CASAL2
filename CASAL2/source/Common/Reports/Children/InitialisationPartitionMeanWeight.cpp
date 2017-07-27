/**
 * @file InitialisationPartitionMeanWeight.cpp
 * @author C.Marsh
 * @date17/07/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "InitialisationPartitionMeanWeight.h"

#include <iostream>
#include <iomanip>

#include "Common/Model/Model.h"
#include "Common/Partition/Accessors/All.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor.
 *
 * Set the run mode and model state for this report
 */
InitialisationPartitionMeanWeight::InitialisationPartitionMeanWeight(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kInitialise;
}

/**
 *
 */
void InitialisationPartitionMeanWeight::DoExecute() {
  LOG_TRACE();
  //  auto categories = Categories::Instance();
  niwa::partition::accessors::All all_view(model_);
  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    (*iterator)->UpdateMeanWeightData();
    (*iterator)->UpdateMeanLengthData();

    string category = (*iterator)->name_;
    cache_ << category << " " << REPORT_R_LIST << "\n";

    cache_ << "mean_weights " << REPORT_R_LIST << "\n";
    cache_ << "values: ";

    for (unsigned age = (*iterator)->min_age_; age <= (*iterator)->max_age_; ++age)
      cache_ << AS_DOUBLE((*iterator)->mean_weight_per_[age]) << " ";
    cache_<<"\n";

    cache_ << REPORT_R_LIST_END <<"\n";


    cache_ << "age_lengths " << REPORT_R_LIST << "\n";
    cache_ << "values: ";

    for (unsigned age = (*iterator)->min_age_; age <= (*iterator)->max_age_; ++age)
      cache_ << AS_DOUBLE((*iterator)->mean_length_per_[age]) << " ";
    cache_<<"\n";

    cache_ << REPORT_R_LIST_END <<"\n";

    cache_ << REPORT_R_LIST_END <<"\n";
  }

  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
