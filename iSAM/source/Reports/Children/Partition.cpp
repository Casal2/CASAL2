/**
 * @file Partition.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Partition.h"

#include <iostream>
#include <iomanip>

#include "Model/Model.h"
#include "Partition/Accessors/All.h"

// Namespaces
namespace niwa {
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

/**
 *
 */
void Partition::DoExecute() {
  // First, figure out the lowest and highest ages/length
  unsigned lowest         = 9999;
  unsigned highest        = 0;
  unsigned longest_length = 0;

  niwa::partition::accessors::All all_view(model_);
  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    if (lowest > (*iterator)->min_age_)
      lowest = (*iterator)->min_age_;
    if (highest < (*iterator)->max_age_)
      highest = (*iterator)->max_age_;
    if (longest_length < (*iterator)->name_.length())
      longest_length = (*iterator)->name_.length();
  }

  /*
  // Print the header
  cache_ << "*" << this->label() << "\n";
  cache_ << "report.type: partition\n";
  cache_ << "year: " << Model::Instance()->current_year() << "\n";
  cache_ << "time_step: " << time_step_ << "\n";
  cache_ << "category";
  for (unsigned i = lowest; i <= highest; ++i)
    cache_ << " " << i;
  cache_ << "\n";

  cache_.precision(1);
  cache_ << std::fixed;

  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    cache_ << (*iterator)->name_;
    unsigned age = (*iterator)->min_age_;
    for (auto values = (*iterator)->data_.begin(); values != (*iterator)->data_.end(); ++values, age++) {
      if (age >= lowest && age <= highest)
        cache_ << " " << std::fixed << std::setprecision(6) << *values;
      else
        cache_ << " " << "null";
    }
    cache_ << "\n";
  }
  ready_for_writing_ = true;
 */

  // Print the header
  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
  cache_ << "year: " << model_->current_year() << "\n";
  cache_ << "time_step: " << time_step_ << "\n";
  cache_ << "values "<< REPORT_R_DATAFRAME<<"\n";
  cache_ << "category";
  for (unsigned i = lowest; i <= highest; ++i)
    cache_ << " " << i;
  cache_ << "\n";

  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    cache_ << (*iterator)->name_;
    unsigned age = (*iterator)->min_age_;
    for (auto values = (*iterator)->data_.begin(); values != (*iterator)->data_.end(); ++values, age++) {
      if (age >= lowest && age <= highest)
        cache_ << " " << std::fixed << *values;
      else
        cache_ << " " << "null";
    }
    cache_ << "\n";
  }
  ready_for_writing_ = true;


}

} /* namespace reports */
} /* namespace niwa */
