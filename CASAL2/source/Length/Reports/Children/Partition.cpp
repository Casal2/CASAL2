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

#include <iostream>
#include <iomanip>

#include "Common/Model/Model.h"
#include "Common/Partition/Accessors/All.h"

// Namespaces
namespace niwa {
namespace length {
namespace reports {

/**
 * Default constructor
 */
Partition::Partition(Model* model) : Report(model) {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kProjection);
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "Time Step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years", "", true);
}

void Partition::DoValidate() {
 if (!parameters_.Get(PARAM_YEARS)->has_been_defined()) {
   years_ = model_->years();
 }
}
/**
 *
 */
void Partition::DoExecute() {

  niwa::partition::accessors::All all_view(model_);
  vector<unsigned> length_bins = model_->length_bins();
  // Print the header
  cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
  cache_ << "year: " << model_->current_year() << "\n";
  cache_ << "time_step: " << time_step_ << "\n";
  cache_ << "values "<< REPORT_R_DATAFRAME<<"\n";
  cache_ << "category";
  for (unsigned length_bin : length_bins)
    cache_ << " " << length_bin;
  cache_ << "\n";

  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    cache_ << (*iterator)->name_;
    for (auto values = (*iterator)->data_.begin(); values != (*iterator)->data_.end(); ++values) {
        Double value = *values;
        cache_ << " " << std::fixed << AS_DOUBLE(value);

    }
    cache_ << "\n";
  }
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace length */
} /* namespace niwa */
