/**
 * @file Categories.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 11/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "Categories.h"

#include "Model/Model.h"
#include "Utilities/Logging/Logging.h"
#include "Utilities/To.h"

// Namespaces
namespace isam {

/**
 * Default constructor
 */
Categories::Categories() {
  LOG_TRACE();
  parameters_.RegisterAllowed(PARAM_FORMAT);
  parameters_.RegisterAllowed(PARAM_NAMES);
  parameters_.RegisterAllowed(PARAM_YEARS);
  parameters_.RegisterAllowed(PARAM_AGES);
}

/**
 * Our singleton accessor method
 *
 * @return singleton shared ptr
 */
shared_ptr<Categories> Categories::Instance() {
  static CategoriesPtr categories = CategoriesPtr(new Categories());
  return categories;
}

/**
 * Validate our Categories
 */
void Categories::Validate() {

  // Check that we actually had a categories block
  if (block_type_ == "")
    LOG_ERROR("The @categories block is missing from the configuration file. This block is required");

  // Check for all required parameters
  CheckForRequiredParameter(PARAM_NAMES);

  /**
   * Parameter: format
   *
   * The format parameter is responsible for allowing us to define how the names
   * of categories are structured. If this is enabled then we can use a short-hand
   * lookup technique to assign values to a category.
   */
  if (parameters_.IsDefined(PARAM_FORMAT)) {
    Parameter parameter = parameters_.Get(PARAM_FORMAT);
    if (parameter.values_.size() > 1)
      LOG_ERROR("At line " << parameter.line_number_ << " in file " << parameter.file_name_
          << ": format can only have 1 parameter specified. E.g format species.sex.stage");
    format_ = parameter.values_[0];
  }

  /**
   * Parameter: Names
   *
   * 1. Verify we have some names present
   * 2. Verify the names match the format string properly (right amount of segments)
   * 3. Create a new CategoryInfo object for each category and store it
   */

  // Parameter: Names
  Parameter parameter = parameters_.Get(PARAM_NAMES);
  names_ = parameter.values_;

  ModelPtr model = Model::Instance();

  vector<unsigned> default_years;
  for (unsigned i = 0; i < model->run_length(); ++i)
    default_years.push_back(model->start_year() + i);

  for (unsigned i = 0; i < names_.size(); ++i) {

    // TODO: Verify the name matches the format string properly

    // Create a new CategoryInfo object
    CategoryInfo new_category_info;
    new_category_info.name_     = names_[i];
    new_category_info.min_age_  = model->min_age();
    new_category_info.max_age_  = model->max_age();
    categories_[names_[i]] = new_category_info;
  }















  // Parameter: Years
  if (parameters_.IsDefined(PARAM_YEARS)) {
    parameter = parameters_.Get(PARAM_YEARS);

  }

  // Parameter: Ages
  if (parameters_.IsDefined(PARAM_AGES)) {
    parameter = parameters_.Get(PARAM_AGES);

  }


}

} /* namespace isam */
















