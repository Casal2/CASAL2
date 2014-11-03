/**
 * @file PartitionMeanWeight.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 7/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "PartitionMeanWeight.h"

#include "AgeSizes/Manager.h"
#include "Categories/Categories.h"
#include "Partition/Partition.h"
#include "Partition/Accessors/All.h"
#include "SizeWeights/Manager.h"

// namespaces
namespace isam {
namespace reports {

/**
 * default constructor
 */
PartitionMeanWeight::PartitionMeanWeight() {
  run_mode_    = RunMode::kBasic;
  model_state_ = State::kFinalise;
}

/**
 * Execute method
 */
void PartitionMeanWeight::DoExecute() {
  cache_ << "*" << this->label() << "\n";

  CategoriesPtr categories = Categories::Instance();
  isam::partition::accessors::All all_view;

  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    string category = (*iterator)->name_;
    cache_ << "category: " << category << "\n";

    cache_ << "mean_weights:\n";
    for (auto iter = (*iterator)->mean_weights_by_year_.begin(); iter != (*iterator)->mean_weights_by_year_.end(); ++iter) {
      cache_ << iter->first <<": ";
      for (Double data : iter->second)
        cache_ << data << " ";
      cache_ << "\n";
    }

    AgeSizePtr age_size = categories->age_size(category);
    cache_ << "age_weight:\n";
    unsigned age_spread = (categories->max_age(category) - categories->min_age(category)) + 1;
    vector<unsigned> years = Model::Instance()->years();

    for (unsigned year : years) {
      cache_ << year << ": ";
      for (unsigned i = 0; i < age_spread; ++i)
        cache_ << age_size->mean_weight(year, i + (*iterator)->min_age_) << " ";
      cache_ << "\n";
    }


    cache_ << "age_size:\n";
    for (unsigned year : years) {
      cache_ << year << ": ";
      for (unsigned i = 0; i < age_spread; ++i)
        cache_ << age_size->mean_size(year, i + (*iterator)->min_age_) << " ";
      cache_ << "\n";
    }
  }

  cache_ << "*end" << endl;
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace isam */
