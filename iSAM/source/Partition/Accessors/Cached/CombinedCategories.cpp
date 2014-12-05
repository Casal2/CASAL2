/**
 * @file CombinedCategories.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// headers
#include "CombinedCategories.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>

// namespaces
namespace niwa {
namespace partition {
namespace accessors {
namespace cached {

/**
 * Default constructor
 */
CombinedCategories::CombinedCategories(const vector<string>& category_labels) {
  LOG_TRACE();
  model_ = Model::Instance();
  LOG_INFO("Categories: " << category_labels.size());

  vector<string> split_category_labels;
  category_labels_.resize(category_labels.size());

  for (unsigned i = 0; i < category_labels.size(); ++i) {
    boost::split(split_category_labels, category_labels[i], boost::is_any_of("+"));
    category_labels_[i] = split_category_labels;
  }
}

/**
 * This method will build our cache. It's
 * essentially a copy of the partition for
 * specific categories at a specific time
 * so it can be used for interpolation later.
 */
void CombinedCategories::BuildCache() {
  data_.clear();
  data_.resize(category_labels_.size());

  unsigned year = model_->current_year();
  Partition& partition = Partition::Instance();

  for (unsigned i = 0; i < category_labels_.size(); ++i) {
    for (auto cat_iter = category_labels_[i].begin(); cat_iter != category_labels_[i].end(); ++cat_iter) {

      partition::Category& category = partition.category(*cat_iter);
      if (std::find(category.years_.begin(), category.years_.end(), year) == category.years_.end())
        continue; // Not valid in this year

      data_[i].push_back(category);
    }

    if (data_[i].size() != category_labels_[i].size())
      LOG_CODE_ERROR("data_[i].size() != category_labels_[i].size() | i = " << i);
  }
}

CombinedCategories::DataType::iterator CombinedCategories::Begin() {
  return data_.begin();
}

CombinedCategories::DataType::iterator CombinedCategories::End() {
  return data_.end();
}

unsigned CombinedCategories::Size() {
  return data_.size();
}

} /* namespace cached */
} /* namespace accessors */
} /* namespace partition */
} /* namespace niwa */
