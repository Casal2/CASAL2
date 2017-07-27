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

#include "Common/Categories/Categories.h"
#include "Common/Partition/Partition.h"
#include "Common/Model/Model.h"
#include "Common/Logging/Logging.h"
#include "Common/Utilities/To.h"

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
Category::Category(Model* model, const string& category_name)
  : model_(model) {
  LOG_TRACE();

  vector<unsigned> years = model_->years();

  Partition& partition = model_->partition();

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
