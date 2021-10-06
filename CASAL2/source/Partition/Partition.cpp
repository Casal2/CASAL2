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

// defines
#define _USE_MATH_DEFINES

// Headers
#include "Partition.h"

#include <cmath>

#include "../AgeLengths/AgeLength.h"
#include "../Categories/Categories.h"
#include "../Logging/Logging.h"
#include "../Model/Model.h"
#include "../Utilities/Math.h"

// Namespaces
namespace niwa {

/**
 * Destructor
 */
Partition::~Partition() {
  for (auto partition : partition_) {
    if (partition.second != nullptr)
      delete partition.second;
  }
}

/**
 * Validate the objects
 */
void Partition::Validate() {}

/**
 * Build the partition structure. This involves getting
 * the category information and building the raw structure.
 *
 * The range of years that each category has will be addressed
 * with the accessor objects.
 */
void Partition::Build() {
  Categories*    categories     = model_->categories();
  vector<string> category_names = categories->category_names();

  for (string category : category_names) {
    LOG_FINE() << "Adding category " << category << " to the partition";

    partition::Category* new_category = new partition::Category(model_);
    new_category->name_               = category;
    new_category->min_age_            = categories->min_age(category);
    new_category->max_age_            = categories->max_age(category);
    new_category->years_              = categories->years(category);
    new_category->age_length_         = categories->age_length(category);

    if (model_->partition_type() == PartitionType::kAge) {
      unsigned age_spread = (categories->max_age(category) - categories->min_age(category)) + 1;
      LOG_FINEST() << "resizing data_ to " << age_spread;
      new_category->data_.resize(age_spread, 0.0);
      new_category->cached_data_.resize(age_spread, 0.0);

    } else if (model_->partition_type() == PartitionType::kLength) {
      unsigned length_bins = model_->length_bins().size();
      new_category->length_data_.resize(length_bins, 0.0);
      new_category->cached_length_data_.resize(length_bins, 0.0);
    }

    partition_[category] = new_category;
  }
}

/**
 * Reset the partition by setting all data values to 0.0
 */
void Partition::Reset() {
  LOG_FINE() << "Partition::Reset()";
  for (auto iter = partition_.begin(); iter != partition_.end(); ++iter) {
    iter->second->data_.assign(iter->second->data_.size(), 0.0);
  }
}

/**
 *  Return a reference to one of the partition categories
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
