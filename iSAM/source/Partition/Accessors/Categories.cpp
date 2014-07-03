/**
 * @file Categories.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/02/2013
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
namespace isam {
namespace partition {
namespace accessors {

/**
 * Default Constructor
 */
Categories::Categories() {

}

void Categories::Init(const vector<string>& category_labels) {
  LOG_TRACE();

  model_ = Model::Instance();
  unsigned start_year = model_->start_year();
  unsigned final_year = model_->final_year();

  if (model_->run_mode() == RunMode::kProjection)
    final_year = model_->projection_final_year();
  LOG_INFO("Model details: start_year: " << start_year << "; final_year: " << final_year);
  LOG_INFO("Categories: " << category_labels.size());

  Partition& partition = Partition::Instance();

  for(string category_label : category_labels) {
    partition::Category& category = partition.category(category_label);
    for (unsigned year = start_year; year <= final_year; ++year) {
      if (std::find(category.years_.begin(), category.years_.end(), year) == category.years_.end())
              continue; // Not valid in this year

      data_[year].push_back(&category);
    }
  }
}

/**
 * Return an iterator to the first object in our container
 * for the current year in the model.
 *
 * @return Iterator to first stored element for current year
 */
Categories::DataType::iterator Categories::begin() {
  return data_[model_->current_year()].begin();
}

/**
 * Return an iterator to the end object in our container
 * for the current year in the model
 *
 * @return End iterator for the stored elements for current year
 */
Categories::DataType::iterator Categories::end() {
  return data_[model_->current_year()].end();
}

/**
 *
 */
unsigned Categories::size() {
  return data_[model_->current_year()].size();
}

} /* namespace accessors */
} /* namespace partition */
} /* namespace isam */
