/**
 * @file PartitionMeanWeight.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 7/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "PartitionMeanWeight.h"

#include "Age/AgeLengths/Manager.h"
#include "Common/Categories/Categories.h"
#include "Common/Model/Model.h"
#include "Common/Partition/Partition.h"
#include "Common/Partition/Accessors/All.h"
#include "Common/LengthWeights/Manager.h"
#include "Common/TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * default constructor
 */
PartitionMeanWeight::PartitionMeanWeight(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection | RunMode::kSimulation| RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "Time Step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years", "", true);
}

/**
 * Build method
 */
void PartitionMeanWeight::DoBuild() {
  if (!parameters_.Get(PARAM_YEARS)->has_been_defined()) {
    years_ = model_->years();
  }
}

/**
 * Execute method
 */
void PartitionMeanWeight::DoExecute() {
  unsigned time_step_index = model_->managers().time_step()->current_time_step();

//  auto categories = Categories::Instance();
  niwa::partition::accessors::All all_view(model_);
  unsigned year = model_->current_year();
  cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
  cache_ << "year: " << year << "\n";
  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {

    string category = (*iterator)->name_;
    cache_ << category << " " << REPORT_R_LIST << "\n";

    cache_ << "mean_weights " << REPORT_R_LIST << "\n";
    cache_ << "values: ";

    for (unsigned age = (*iterator)->min_age_; age <= (*iterator)->max_age_; ++age)
      cache_ << AS_DOUBLE((*iterator)->mean_weight_by_time_step_age_[time_step_index][age]) << " ";
    cache_<<"\n";

    cache_ << REPORT_R_LIST_END <<"\n";


    cache_ << "age_lengths " << REPORT_R_LIST << "\n";
    cache_ << "values: ";

    for (unsigned age = (*iterator)->min_age_; age <= (*iterator)->max_age_; ++age)
      cache_ << AS_DOUBLE((*iterator)->mean_length_by_time_step_age_[time_step_index][age]) << " ";
    cache_<<"\n";

    cache_ << REPORT_R_LIST_END <<"\n";

    cache_ << REPORT_R_LIST_END <<"\n";
  }


  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
