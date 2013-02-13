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

    partition_[category] = new_category;
  }
}

} /* namespace isam */
