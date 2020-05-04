/**
 * @file PartitionMeanLength.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 7/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "PartitionMeanLength.h"

#include "AgeLengths/Manager.h"
#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Partition/Partition.h"
#include "Partition/Accessors/All.h"
#include "TimeSteps/Manager.h"

// namespaces
namespace niwa {
namespace reports {
namespace age {

/**
 * Default constructor
 */
PartitionMeanLength::PartitionMeanLength(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection | RunMode::kSimulation | RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "Time Step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years", "", true);
}

/**
 * Build method
 */
void PartitionMeanLength::DoBuild() {
  if (!parameters_.Get(PARAM_YEARS)->has_been_defined()) {
    years_ = model_->years();
  }
}

/**
 * Execute method
 */
void PartitionMeanLength::DoExecute() {
  LOG_TRACE();
  unsigned time_step_index = model_->managers().time_step()->current_time_step();

//  auto categories = Categories::Instance();
  niwa::partition::accessors::All all_view(model_);
  unsigned year       = model_->current_year();
  unsigned year_index = year > model_->start_year() ? year - model_->start_year() : 0;
  if (find(years_.begin(),years_.end(), year) != years_.end()) {
    cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
    cache_ << "year: " << year << "\n";
    for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
      string category = (*iterator)->name_;
      LOG_FINEST() << "printing mean length for category " << category;
      cache_ << category << " " << REPORT_R_LIST << "\n";

      cache_ << "mean_lengths " << REPORT_R_LIST << "\n";
      cache_ << "values: ";

      unsigned age_bins = (*iterator)->age_spread();
      for (unsigned age_index = 0; age_index < age_bins; ++age_index) {
        Double temp = (*iterator)->mean_length_by_time_step_age_[year_index][time_step_index][age_index];
        cache_ << AS_VALUE(temp) << " ";
      }
      cache_<<"\n";
      LOG_FINEST() << "cached mean length";
      cache_ << REPORT_R_LIST_END <<"\n";

      cache_ << REPORT_R_LIST_END <<"\n";
    }

    ready_for_writing_ = true;
  }

}

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */
