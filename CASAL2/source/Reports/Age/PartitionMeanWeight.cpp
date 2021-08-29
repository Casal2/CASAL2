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
namespace age {

/**
 * Default constructor
 */
PartitionMeanWeight::PartitionMeanWeight() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection | RunMode::kSimulation | RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = (State::Type)(State::kIterationComplete);

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
 * Execute the report
 */
void PartitionMeanWeight::DoExecute(shared_ptr<Model> model) {
  LOG_TRACE();

  //  unsigned year_index      = 0;
  unsigned                        time_step_index = model->managers()->time_step()->GetTimeStepIndex(time_step_);
  niwa::partition::accessors::All all_view(model);

  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "time_step: " << time_step_ << REPORT_EOL;

  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    string category = (*iterator)->name_;
    LOG_FINEST() << "printing mean weight-at-age for category " << category;
    cache_ << category << " " << REPORT_R_LIST << REPORT_EOL;
    cache_ << "mean_weights " << REPORT_R_DATAFRAME << REPORT_EOL;
    cache_ << "year ";
    for (unsigned i = model->min_age(); i <= model->max_age(); ++i) cache_ << i << " ";
    cache_ << REPORT_EOL;

    for (auto year : years_) {
      //      year_index = year > model_->start_year() ? year - model_->start_year() : 0;
      cache_ << year << " ";

      for (unsigned age = (*iterator)->min_age_; age <= (*iterator)->max_age_; ++age) {
        Double temp = (*iterator)->mean_weight_by_time_step_age_[time_step_index][age];
        cache_ << AS_DOUBLE(temp) << " ";
      }
      cache_ << REPORT_EOL;
      LOG_FINEST() << "cached mean weight";
    }
    cache_ << REPORT_R_LIST_END << REPORT_EOL;
  }
  ready_for_writing_ = true;
}

} /* namespace age */
} /* namespace reports */
} /* namespace niwa */
