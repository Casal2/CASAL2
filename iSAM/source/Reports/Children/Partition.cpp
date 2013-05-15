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
namespace isam {
namespace reports {

/**
 * Default constructor
 */
Partition::Partition() {
  run_mode_    = RunMode::kBasic;
  model_state_ = State::kExecute;
}

/**
 *
 */
void Partition::Execute() {

  // First, figure out the lowest and highest ages/length
  unsigned lowest         = 9999;
  unsigned highest        = 0;
  unsigned longest_length = 0;

  isam::partition::accessors::All all_view;
  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    if (lowest > (*iterator)->min_age_)
      lowest = (*iterator)->min_age_;
    if (highest < (*iterator)->max_age_)
      highest = (*iterator)->max_age_;
    if (longest_length < (*iterator)->name_.length())
      longest_length = (*iterator)->name_.length();
  }

  // Print the header
  cout << "*" << this->label() << "\n";
  cout << "report.type: partition\n";
  cout << "year: " << Model::Instance()->current_year() << "\n";
  cout << "time_step: " << time_step_ << "\n";
  cout << "category";
  for (unsigned i = lowest; i <= highest; ++i)
    cout << " " << i;
  cout << "\n";

  cout.precision(1);
  cout << std::fixed;

  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    cout << (*iterator)->name_;
    unsigned age = (*iterator)->min_age_;
    for (auto values = (*iterator)->data_.begin(); values != (*iterator)->data_.end(); ++values, age++) {
      if (age >= lowest && age <= highest)
        cout << " " << std::fixed << std::setprecision(6) << *values;
      else
        cout << " " << "null";
    }
    cout << "\n";
  }
  cout << "*end" << endl << endl;
}


} /* namespace reports */
} /* namespace isam */
