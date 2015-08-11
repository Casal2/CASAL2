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
#include "Logging/Logging.h"
#include "Utilities/To.h"

// Namespaces
namespace niwa {
namespace partition {
namespace accessors {

using std::cout;
using std::endl;
namespace util = niwa::utilities;

/**
 * Default constructor.
 *
 * Construction of this object involves building the category map
 */
Category::Category(const string& category_name) {
  LOG_TRACE();

  model_ = Model::Instance();
  vector<unsigned> years = model_->years();

  Partition& partition = Partition::Instance();

  partition::Category& category = partition.category(category_name);
  for (unsigned year : years) {
    if (std::find(category.years_.begin(), category.years_.end(), year) == category.years_.end())
            continue; // Not valid in this year

    data_[year] = &category.data_;
  }
}

} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
