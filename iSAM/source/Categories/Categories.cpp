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

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>

#include "AgeSizes/Manager.h"
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
  parameters_.RegisterAllowed(PARAM_AGE_SIZES);
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
    format_ = parameter.GetValue<string>();
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
  names_ = parameter.values();

  ModelPtr model = Model::Instance();

  // build the default years
  vector<unsigned> default_years;
  for (unsigned i = model->start_year(); i <= model->final_year(); ++i)
    default_years.push_back(i);

  // get the age sizes
  if (parameters_.IsDefined(PARAM_AGE_SIZES)) {
    age_size_labels_ = parameters_.Get(PARAM_AGE_SIZES).GetValues<string>();
    if (age_size_labels_.size() != names_.size())
      LOG_ERROR(parameters_.location(PARAM_AGE_SIZES) << " number defined (" << age_size_labels_.size() << ") must be the same as the number " <<
          " of categories defined (" << names_.size() << ")");
  }

  // build our categories vector
  for (unsigned i = 0; i < names_.size(); ++i) {
    if (age_size_labels_.size() > i)
      category_age_size_labels_[names_[i]] = age_size_labels_[i];

    // TODO: Verify the name matches the format string properly
    // TODO: Expand the names

    // Create a new CategoryInfo object
    CategoryInfo new_category_info;
    new_category_info.name_     = names_[i];
    new_category_info.min_age_  = model->min_age();
    new_category_info.max_age_  = model->max_age();
    new_category_info.years_    = default_years;
    categories_[names_[i]] = new_category_info;

    category_names_.push_back(names_[i]);
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

/**
 * Build runtime relationships between the categories
 * and other objects in the system
 */
void Categories::Build() {
  agesizes::Manager& age_sizes_manager = agesizes::Manager::Instance();

  auto iter = category_age_size_labels_.begin();
  for (; iter != category_age_size_labels_.end(); ++iter) {
    AgeSizePtr age_size = age_sizes_manager.GetAgeSize(iter->second);
    if (!age_size)
      LOG_ERROR(parameters_.location(PARAM_AGE_SIZES) << "(" << iter->second << ") could not be found. Have you defined it?");

    cout << "Loading age_size for category: " << iter->first << endl;
    categories_[iter->first].age_size_ = age_size;
  }
}

/*
 *
 */
void Categories::Reset() {
}

/**
 * Check if the parameter category label is a valid
 * category or not.
 */
bool Categories::IsValid(const string& label) const {
  return std::find(category_names_.begin(), category_names_.end(), label) != category_names_.end();
}

/**
 *
 */
bool Categories::IsCombinedLabels(const string& label) const {
  return (label.find_first_of("+") != string::npos);
}

/**
 *
 */
unsigned Categories::GetNumberOfCategoriesDefined(const string& label) const {
  vector<string> category_labels;
  boost::split(category_labels, label, boost::is_any_of("+"));

  return category_labels.size();
}

/**
 *
 */
unsigned Categories::min_age(const string& category_name) {
  if (categories_.find(category_name) == categories_.end())
    LOG_CODE_ERROR("Could not find category_name: " << category_name << " in the list of loaded categories");

  return categories_[category_name].min_age_;
}

/**
 *
 */
unsigned Categories::max_age(const string& category_name) {
  if (categories_.find(category_name) == categories_.end())
    LOG_CODE_ERROR("Could not find category_name: " << category_name << " in the list of loaded categories");

  return categories_[category_name].max_age_;
}

/**
 *
 */
vector<unsigned> Categories::years(const string& category_name) {
  if (categories_.find(category_name) == categories_.end())
    LOG_CODE_ERROR("Could not find category_name: " << category_name << " in the list of loaded categories");

  return categories_[category_name].years_;
}

/**
 *
 */
AgeSizePtr Categories::age_size(const string& category_name) {
  if (categories_.find(category_name) == categories_.end())
    LOG_CODE_ERROR("Could not find category_name: " << category_name << " in the list of loaded categories");
  if (!categories_[category_name].age_size_)
    LOG_CODE_ERROR("The age size pointer was null for category " << category_name);

  return categories_[category_name].age_size_;
}

/**
 * This method will remove all of the information from our categories
 */
void Categories::RemoveAllObjects() {
  parameters_.Clear();
  format_ = "";
  names_.clear();
  category_names_.clear();
  categories_.clear();
  age_size_labels_.clear();
  category_age_size_labels_.clear();
}


} /* namespace isam */
















