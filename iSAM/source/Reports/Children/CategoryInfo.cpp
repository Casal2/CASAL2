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
  model_state_ = State::kFinalise;
}

/**
 *
 */
void CategoryInfo::Prepare() {}

/**
 *
 */
void CategoryInfo::Execute() {

  cout << "*category_info: " << label_ << "\n";

  CategoriesPtr categories = Categories::Instance();

  vector<string> names = categories->category_names();
  for(string name : names) {
    cout << "Category: " << name << "\n";
    cout << "min_age: " << categories->min_age(name) << "\n";
    cout << "max_age: " << categories->max_age(name) << "\n";

    vector<unsigned> years = categories->years(name);
    cout << "years: ";
    for (unsigned year : years)
      cout << year << " ";
    cout << "\n\n";
  }

  cout << "*end" << endl;
}

/**
 *
 */
void CategoryInfo::Finalise() {}

} /* namespace reports */
} /* namespace isam */
