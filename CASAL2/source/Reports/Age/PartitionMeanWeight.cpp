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

#include "AgeLengths/Manager.h"
#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Partition/Partition.h"
#include "Partition/Accessors/All.h"
#include "LengthWeights/Manager.h"
#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace reports {
namespace age {

/**
 * default constructor
 */
PartitionMeanWeight::PartitionMeanWeight(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection | RunMode::kSimulation | RunMode::kEstimation | RunMode::kProfiling);
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
  LOG_TRACE();
  unsigned time_step_index = model_->managers().time_step()->current_time_step();

//  auto categories = Categories::Instance();
  niwa::partition::accessors::All all_view(model_);
  unsigned year = model_->current_year();
  if (find(years_.begin(),years_.end(), year) != years_.end()) {
    cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
    cache_ << "year: " << year << "\n";
    for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
      string category = (*iterator)->name_;
      LOG_FINEST() << "printing mean weight for category " << category;
      cache_ << category << " " << REPORT_R_LIST << "\n";

      cache_ << "mean_weights " << REPORT_R_LIST << "\n";
      cache_ << "values: ";

      for (unsigned age = (*iterator)->min_age_; age <= (*iterator)->max_age_; ++age) {
        Double temp = (*iterator)->mean_weight_by_time_step_age_[time_step_index][age]; // Sometimes the AS_DOUBLE Macro can be a pain in the ass.
        cache_ << AS_DOUBLE(temp) << " ";
      }
      cache_<<"\n";
      LOG_FINEST() << "cached mean weight";
      cache_ << REPORT_R_LIST_END <<"\n";

/*
      // This currently doesn't work
      cache_ << "age_lengths " << REPORT_R_LIST << "\n";
      cache_ << "values: ";

      for (unsigned age = (*iterator)->min_age_; age <= (*iterator)->max_age_; ++age) {
        Double temp1 = (*iterator)->mean_length_by_time_step_age_[time_step_index][age];
        cache_ << AS_DOUBLE(temp1) << " ";
      }
      LOG_FINEST() << "cached mean length";

      cache_<<"\n";

      cache_ << REPORT_R_LIST_END <<"\n";
*/
      cache_ << REPORT_R_LIST_END <<"\n";
    }

    ready_for_writing_ = true;
  }

}

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */
