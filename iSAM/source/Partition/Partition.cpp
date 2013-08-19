/**
 * @file Partition.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 7/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Partition.h"

#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Utilities/Logging/Logging.h"

// Namespaces
namespace isam {

/**
 *
 */
Partition& Partition::Instance() {
  static Partition instance = Partition();
  return instance;
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
  ModelPtr model                            = Model::Instance();
  CategoriesPtr categories                  = Categories::Instance();
  vector<string> category_names             = categories->category_names();

  for(string category : category_names) {
    LOG_INFO("Adding category " << category << " to the partition");

    partition::Category new_category;
    new_category.name_      = category;
    new_category.min_age_   = categories->min_age(category);
    new_category.max_age_   = categories->max_age(category);
    new_category.years_     = categories->years(category);

    unsigned age_spread = (categories->max_age(category) - categories->min_age(category)) + 1;
    new_category.data_.resize(age_spread, 0.0);

    new_category.mean_weights_.resize(age_spread, 0.0);
    for (unsigned i = 0; i < age_spread; ++i)
      new_category.mean_weights_[i] = categories->age_size(category)->mean_weight(i + new_category.min_age_);

    partition_[category] = new_category;
  }
}

/**
 * Reset our partition so all data values are 0.0
 */
void Partition::Reset() {
  for (auto iter = partition_.begin(); iter != partition_.end(); ++iter) {
    iter->second.data_.assign(iter->second.data_.size(), 0.0);
  }
}

/**
 *  This method will return a reference to one of our partition categories.
 *
 *  @param category_label The name of the category
 */
partition::Category& Partition::category(const string& category_label) {
  auto find_iter = partition_.find(category_label);
  if (find_iter == partition_.end())
    LOG_CODE_ERROR("The partition does not have a category " << category_label);

  return find_iter->second;
}

/**
 * This method will print the contents of the partition out to stdout.
 * It's useful when trying to debug why the partition may not be working properly
 */
void Partition::Debug() {
  cout << "-- Partition Debug -- " << endl;
  for (auto iter = partition_.begin(); iter != partition_.end(); ++iter) {
    cout << "Category: " << iter->first << endl;
    cout << "min_age: " << iter->second.min_age_ << endl;
    cout << "max_age: " << iter->second.max_age_ << endl;
    cout << "years: ";
    for (unsigned i = 0; i < iter->second.years_.size(); ++i)
      cout << iter->second.years_[i] << ", ";
    cout << endl;
    cout << "data: ";
    for (unsigned i = 0; i < iter->second.data_.size(); ++i)
      cout << AS_DOUBLE(iter->second.data_[i]) << ", ";
    cout << endl;
  }
  cout << "-- End Partition Debug -- " << endl;
}

} /* namespace isam */
