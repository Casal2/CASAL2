/**
 * @file Category.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 29/10/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Category.h"

#include <iostream>
#include <iomanip>

#include "Categories/Categories.h"
#include "Partition/Partition.h"
#include "Model/Model.h"
#include "Utilities/Logging/Logging.h"
#include "Utilities/To.h"

// Namespaces
namespace isam {
namespace partition {
namespace accessors {

using std::cout;
using std::endl;
namespace util = isam::utilities;

/**
 * Default constructor.
 *
 * Construction of this object involves building the category map
 */
Category::Category(const string& category_name) {
  LOG_TRACE();

  model_ = Model::Instance();
  unsigned start_year = model_->start_year();
  unsigned final_year = model_->final_year();
  LOG_INFO("Model details: start_year: " << start_year << "; final_year: " << final_year);

  CategoriesPtr categories = Categories::Instance();


  Partition& partition = Partition::Instance();
  partition::Category& category = partition.category(category_name);
  for (unsigned year = start_year; year <= final_year; ++year) {
    if (std::find(category.years_.begin(), category.years_.end(), year) == category.years_.end())
            continue; // Not valid in this year

    data_[year] = &category.data_;
  }
}

/**
 * This method will return an iterator to the
 * first category for the current year.
 *
 * @return Iterator to the first element for the current year
 */
//Category::DataType::iterator Category::begin() {
//  return category_map_[model_->current_year()].begin();
//}

/**
 * This method returns the end iterator for comparison.
 *
 * @return Iterator that is the end of the current year for comparison
 */
//Category::DataType::iterator Category::end() {
//  return category_map_[model_->current_year()].end();
//}

} /* namespace accessors */
} /* namespace partition */
} /* namespace isam */
