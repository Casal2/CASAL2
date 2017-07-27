/**
 * @file Partition.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 7/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Partition.h"

#include "Age/AgeLengths/AgeLength.h"
#include "Common/Categories/Categories.h"
#include "Common/Model/Model.h"
#include "Common/Logging/Logging.h"

// Namespaces
namespace niwa {

/**
 *
 */
Partition::~Partition() {
  for (auto partition : partition_) {
    if (partition.second != nullptr)
      delete partition.second;
  }
}

/**
 *
 */
void Partition::Validate() {
}

/**
 * Build our partition structure now. This involves getting
 * the category information and building the raw structure.
 *
 * We're not interested in the range of years that each
 * category has because this will be addressed with the
 * accessor objects.
 */
void Partition::Build() {
  Categories* categories                    = model_->categories();
  vector<string> category_names             = categories->category_names();

  for(string category : category_names) {
    LOG_FINEST() << "Adding category " << category << " to the partition";

    partition::Category* new_category = new partition::Category(model_);
    new_category->name_       = category;
    new_category->min_age_    = categories->min_age(category);
    new_category->max_age_    = categories->max_age(category);
    new_category->years_      = categories->years(category);
    new_category->age_length_ = categories->age_length(category);

    unsigned age_spread = (categories->max_age(category) - categories->min_age(category)) + 1;
    new_category->data_.resize(age_spread, 0.0);

    partition_[category] = new_category;
  }
}

/**
 * Reset our partition so all data values are 0.0
 */
void Partition::Reset() {
  for (auto iter = partition_.begin(); iter != partition_.end(); ++iter) {
    iter->second->data_.assign(iter->second->data_.size(), 0.0);
  }
}

/**
 *  This method will return a reference to one of our partition categories.
 *
 *  @param category_label The name of the category
 *  @return reference to the category
 */
partition::Category& Partition::category(const string& category_label) {
  auto find_iter = partition_.find(category_label);
  if (find_iter == partition_.end())
    LOG_FATAL() << "The partition does not have a category " << category_label;

  return (*find_iter->second);
}
} /* namespace niwa */
