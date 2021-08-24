/**
 * @file Partition.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Partition.h"

#include <iomanip>
#include <iostream>

#include "../../Model/Model.h"
#include "../../Partition/Accessors/All.h"

// Namespaces
namespace niwa {
namespace reports {
namespace length {

/**
 * Default constructor
 */
Partition::Partition() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "Time Step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years", "", true);
}

/**
 * Validate
 */
void Partition::DoValidate(shared_ptr<Model> model) {
  if (!parameters_.Get(PARAM_YEARS)->has_been_defined()) {
    years_ = model->years();
  }
}
/**
 *
 */
void Partition::DoExecute(shared_ptr<Model> model) {
  niwa::partition::accessors::All all_view(model);
  vector<Double>                  length_bins = model->length_bins();
  // Print the header
  cache_ << ReportHeader(type_, label_);
  cache_ << "year: " << model->current_year() << REPORT_EOL;
  cache_ << "time_step: " << time_step_ << REPORT_EOL;
  cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;
  cache_ << "category";
  for (auto length_bin : length_bins) cache_ << " " << length_bin;
  cache_ << REPORT_EOL;

  for (auto iterator : all_view) {
    cache_ << iterator->name_;
    for (auto value : iterator->data_) {
      cache_ << " " << std::fixed << AS_DOUBLE(value);
    }
    cache_ << REPORT_EOL;
  }
  ready_for_writing_ = true;
}

} /* namespace length */
} /* namespace reports */
} /* namespace niwa */
