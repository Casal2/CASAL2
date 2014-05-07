/**
 * @file CategoryInfo.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "CategoryInfo.h"

#include "Categories/Categories.h"

// Namespaces
namespace isam {
namespace reports {

/**
 * Default constructor
 */
CategoryInfo::CategoryInfo() {
  run_mode_    = RunMode::kBasic;
  model_state_ = State::kFinalise;
}

/**
 *
 */
void CategoryInfo::Execute() {

  cache_ << "*category_info: " << label_ << "\n";

  CategoriesPtr categories = Categories::Instance();

  vector<string> names = categories->category_names();
  for(string name : names) {
    cache_ << "Category: " << name << "\n";
    cache_ << "min_age: " << categories->min_age(name) << "\n";
    cache_ << "max_age: " << categories->max_age(name) << "\n";

    vector<unsigned> years = categories->years(name);
    cache_ << "years: ";
    for (unsigned year : years)
      cache_ << year << " ";
    cache_ << "\n\n";
  }

  cache_ << "*end" << endl;
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace isam */
