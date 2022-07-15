/**
 * @file CombinedCategories.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "CombinedCategories.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>

// namespaces
namespace niwa {
namespace partition {
namespace accessors {

/**
 * Default constructor
 *
 * @param category_labels List of category labels to parse
 */
CombinedCategories::CombinedCategories(shared_ptr<Model> model, const vector<string>& category_labels) : model_(model) {
  LOG_TRACE();

  unsigned start_year = model_->start_year();
  unsigned final_year = model_->final_year();
  LOG_FINEST() << "Model details: start_year: " << start_year << "; final_year: " << final_year;
  LOG_FINEST() << "Categories: " << category_labels.size();

  Partition&     partition = model_->partition();
  vector<string> split_category_labels;

  for (unsigned year = start_year; year <= final_year; ++year) data_[year].resize(category_labels.size());


  /**
   * Loop through the category labels
   */
  category_count_ = 0;
  for (unsigned i = 0; i < category_labels.size(); ++i) {
    unsigned combined_index = 0;

    // Split the labels in to the individual labels
    boost::split(split_category_labels, category_labels[i], boost::is_any_of("+"));

    // Loop through our split categories now and put them into the data object
    for (string category_label : split_category_labels) {
      partition::Category& category = partition.category(category_label);
      category_labels_.push_back(category_label);
      group_index_.push_back(i);
      combined_index_.push_back(combined_index);
      LOG_FINE() << "caching category " << category_label << " with group_ndx = " << i << " and combined ndx = " << combined_index;
      ++category_count_;
      for (unsigned year = start_year; year <= final_year; ++year) {
        if (std::find(category.years_.begin(), category.years_.end(), year) == category.years_.end())
          continue;  // Not valid in this year


        data_[year][i].push_back(&category);
      }
      combined_index++;
    }
  }
}

/**
 * Get the first element from our collection
 * for the current model year
 */
CombinedCategories::DataType::iterator CombinedCategories::Begin() {
  return data_[model_->current_year()].begin();
}

/**
 * Get the end value for our collection
 * for the current model year
 *
 * @return iterator to end
 */
CombinedCategories::DataType::iterator CombinedCategories::End() {
  return data_[model_->current_year()].end();
}

/**
 * Get the number of category collections
 * for the current year
 *
 * @return number of category collections for current year
 */
unsigned CombinedCategories::Size() {
  return data_[model_->current_year()].size();
}

/**
 * Get a specific category from CombinedCategories
 * using an index, for the current year
 *
 * @return a pointer to a specific category for current year
 */
partition::Category* CombinedCategories::GetCategoryFromCombinedCategoryByIndex(unsigned group_index, unsigned combined_index) {
  return data_[model_->current_year()][group_index][combined_index];
}

/**
 * Get a group index and combined index for a specific category label. This is so other classes 
 * can cache the index to do specific lookups
 * on a category during Execute or PreExecute.
 * @param category_label a specific category label
 * @return a vector of indicies first element is the group_index_, second element is the combined_index_
 */
vector<unsigned> CombinedCategories::get_category_index(string category_label) {
  auto it = std::find(category_labels_.begin(), category_labels_.end(), category_label);
  // could if this category is available
  if (it == category_labels_.end()) {
    LOG_FINEST() << "category_label = " << category_label;
    for(unsigned i = 0; i < category_labels_.size(); ++i)
      LOG_FINEST() << category_labels_[i];
    LOG_CODE_ERROR() << "std::find(category_labels_.begin(), category_labels_.end(), category_label) == category_labels_.end()";
  }
  unsigned category_ndx = distance(category_labels_.begin(), it);
  vector<unsigned> result = {group_index_[category_ndx], combined_index_[category_ndx]};
  return result;
}

} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
