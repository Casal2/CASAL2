/**
 * @file All.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 10/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "All.h"

#include "Categories/Categories.h"
#include "Partition/Partition.h"

// Namespaces
namespace niwa {
namespace partition {
namespace accessors {

/**
 * Default constructor
 */
All::All(Model* model) : model_(model) {

  vector<string> category_names = model_->categories()->category_names();
  for (string category_name : category_names) {
    data_.push_back(&model_->partition().category(category_name));
  }
}

All::DataType::iterator All::Begin() {
  return data_.begin();
}

All::DataType::iterator All::End() {
  return data_.end();
}

unsigned All::Size() {
  return data_.size();
}

} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
