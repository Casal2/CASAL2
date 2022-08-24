/**
 * @file Mortality.cpp
 * @author  C Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */

// Headers
#include "Mortality.h"
#include "Utilities/Vector.h"


// Namespaces
namespace niwa {
namespace processes {
namespace age {

/**
 * Default constructor
 */
Mortality::Mortality(shared_ptr<Model> model) : Process(model) {
  process_type_        = ProcessType::kMortality;
  partition_structure_ = PartitionType::kAge;

}


/**
 * Check the categories in methods for removal obs
 * @description method checks if there is a category in each method, to make sure the observation class is compatable with the process
 * @param methods a vector of methods
 * @param category_labels a vector of categories to check.
 */
bool Mortality::check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels) {
  LOG_TRACE();
  unsigned fishery_index = 0;
  for (; fishery_index < methods.size(); ++fishery_index) {
    if(fishery_category_check_[methods[fishery_index]].size() <= 0) {
      LOG_FINE() << "could not find fishery = " << methods[fishery_index];
      return false;
    }
    for (unsigned category_index = 0; category_index < category_labels.size(); ++category_index) {
      if(find(fishery_category_check_[methods[fishery_index]].begin(), fishery_category_check_[methods[fishery_index]].end(), category_labels[category_index]) == fishery_category_check_[methods[fishery_index]].end()) {
        LOG_FINE() << "Could not find category  = " << category_labels[category_index] << " in fishery = " << methods[fishery_index];// << " categories supplies = " << category_labels.size();
        return false;     
      }
    }
  }
  return true;
}

/**
 * Check the years in methods for removal obs
 * @description method checks if there is a category in each method for each year, to make sure the observation class is compatable with the process
 * @param years a vector of years
 * @param methods a vector of methods
 */
bool Mortality::check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods) {
  for (unsigned fishery_index = 0; fishery_index < methods.size(); ++fishery_index) {
    for(auto year : years) {
      if(fishery_catch_[methods[fishery_index]][year] <= 0) {
        LOG_FINE() << "year = " << year << " fishery = " << methods[fishery_index] << " no catch";
        return false;
      }
    }
  }
  return true;
}

/**
 * Check the categories in methods for removal obs
 * @description method checks if each method exists, to make sure the observation class is compatable with the process
 * @param methods a vector of methods
 */
bool Mortality::check_methods_for_removal_obs(vector<string> methods) {
  LOG_TRACE();
  unsigned method_counter = 0;
  for (unsigned fishery_index = 0; fishery_index < methods.size(); ++fishery_index) {
    if(find(fishery_labels_.begin(), fishery_labels_.end(), methods[fishery_index]) != fishery_labels_.end())
      ++method_counter;
  }
  if (method_counter != methods.size())
    return false;

  return true;
}
/**
 * for a set of given years return the ndx for the catch at object.
 * This should only be called in the build, and then during execute
 * observations will query this class with these indicies using the
 * accessor get_catch_at_by_year_fishery_category()
 * this function assumes all fishery labs have been passed.
 */
vector<unsigned> Mortality::get_fishery_ndx_for_catch_at(vector<string> fishery_labels) {
  vector<unsigned> fishery_ndxs;
  for (unsigned fishery_ndx = 0; fishery_ndx < fishery_labels.size(); ++fishery_ndx) {
    std::pair<bool, int> this_fishery_iter = niwa::utilities::findInVector(fishery_labels_, fishery_labels[fishery_ndx]);
    if (!this_fishery_iter.first) {
      LOG_CODE_ERROR() << "couldn't find fishery label = " << fishery_labels[fishery_ndx] << " in fishery_labels_. this should be validated before this function is used.";
    }
    fishery_ndxs.push_back(this_fishery_iter.second);
  }
  return fishery_ndxs;
}

/**
 * for a set of given years return the ndx for the catch at object.
 * This should only be called in the build, and then during execute
 * observations will query this class with these indicies using the
 * accessor get_catch_at_by_year_fishery_category()
 * this function assumes all fishery labs have been passed.
 */
vector<unsigned> Mortality::get_year_ndx_for_catch_at(vector<unsigned> years) {
  vector<unsigned> year_ndxs;
  for (unsigned year_ndx = 0; year_ndx < years.size(); ++year_ndx) {
    std::pair<bool, int> this_year_iter = niwa::utilities::findInVector(process_years_, years[year_ndx]);
    if (!this_year_iter.first) {
      LOG_CODE_ERROR() << "couldn't find year = " << years[year_ndx] << " in process_years_. this should be validated before this function is used.";
    }
    year_ndxs.push_back(this_year_iter.second);
  }
  return year_ndxs;
}
/**
 * for a set of given years return the ndx for the catch at object.
 * This should only be called in the build, and then during execute
 * observations will query this class with these indicies using the
 * accessor get_catch_at_by_year_fishery_category()
 * this function assumes all fishery labs have been passed.
 */
vector<unsigned> Mortality::get_category_ndx_for_catch_at(vector<string> category_labels) {
  vector<unsigned> category_ndxs;
  for (unsigned category_ndx = 0; category_ndx < category_labels.size(); ++category_ndx) {
    std::pair<bool, int> this_category_iter = niwa::utilities::findInVector(category_labels_, category_labels[category_ndx]);
    if (!this_category_iter.first) {
      LOG_CODE_ERROR() << "couldn't find category label = " << category_labels[category_ndx] << " in category_labels_. this should be validated before this function is used.";
    }
    category_ndxs.push_back(this_category_iter.second);
  }
  return category_ndxs;
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
