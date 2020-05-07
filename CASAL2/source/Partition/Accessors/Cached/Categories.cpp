/**
 * @file Categories.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Categories.h"

#include "Categories/Categories.h"
#include "Partition/Partition.h"

// Namespaces
namespace niwa {
namespace partition {
namespace accessors {
namespace cached {

/**
 * Default Constructor
 */
Categories::Categories(Model* model) : model_(model) { }

/**
 * Initialise
 */
void Categories::Init(const vector<string>& category_labels) {
  LOG_FINEST() << "Categories: " << category_labels.size();
  category_labels_ = category_labels;
}

/**
 * Build a cache of the partition for the current year. Every time this method
 * is called it will overwrite the previous cache.
 */
void Categories::BuildCache() {
  data_.clear();

  unsigned year = model_->current_year();
  Partition& partition = model_->partition();
  for(string category_label : category_labels_) {
    partition::Category& category = partition.category(category_label);
    LOG_FINEST() << "Category: " << category_label << " has " << category.years_.size() << " years loaded";

    if (std::find(category.years_.begin(), category.years_.end(), year) == category.years_.end())
      continue; // Not valid in this year

    data_.push_back(category);
  }
}

/**
 * Return an iterator to the first object in the container
 * @return Iterator to the first stored element
 */
Categories::DataType::iterator Categories::begin() {
  return data_.begin();
}

/**
 * Return an iterator to the end object in the container
 * @return End iterator for the stored elements
 */
Categories::DataType::iterator Categories::end() {
  return data_.end();
}

/**
 * Return the size of the container for the current year. This
 * returns the number of active categories
 * @return The number of active categories
 */
unsigned Categories::size() {
  return data_.size();
}

} /* namespace cached */
} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
