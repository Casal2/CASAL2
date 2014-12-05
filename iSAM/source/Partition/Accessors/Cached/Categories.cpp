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
Categories::Categories(const vector<string>& category_labels) {
  LOG_TRACE();
  model_ = Model::Instance();
  LOG_INFO("Categories: " << category_labels.size());
  category_labels_ = category_labels;
}

/**
 * Build a cache of the partition for the current year. Everytime this method
 * is called it will overwrite the previous cache.
 */
void Categories::BuildCache() {
  data_.clear();

  unsigned year = model_->current_year();
  Partition& partition = Partition::Instance();
  for(string category_label : category_labels_) {
    partition::Category& category = partition.category(category_label);
    LOG_INFO("Category: " << category_label << " has " << category.years_.size() << " years loaded");

    if (std::find(category.years_.begin(), category.years_.end(), year) == category.years_.end())
      continue; // Not valid in this year

    data_.push_back(category);
  }

  if (data_.size() != category_labels_.size())
    LOG_CODE_ERROR("Number of categories loaded is not the same as the number provided (" << data_.size() << " != " << category_labels_.size() << ")");

}

/**
 * Return an iterator to the first object in our container
 *
 * @return Iterator to first stored element
 */
Categories::DataType::iterator Categories::Begin() {
  return data_.begin();
}

/**
 * Return an iterator to the end object in our container
 *
 * @return End iterator for the stored elements
 */
Categories::DataType::iterator Categories::End() {
  return data_.end();
}

/**
 * Return the size of our container for the current year. This
 * will return the number of active categories
 *
 * @return The number of active categories
 */
unsigned Categories::Size() {
  return data_.size();
}

} /* namespace cached */
} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
