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
void PartitionMeanWeight::Execute() {
  cout << "*" << this->label() << "\n";

  CategoriesPtr categories = Categories::Instance();
  isam::partition::accessors::All all_view;

  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    string category = (*iterator)->name_;
    cout << "category: " << category << "\n";

    cout << "mean_weights: ";
    for (unsigned i = 0; i < (*iterator)->mean_weights_.size(); ++i)
      cout << (*iterator)->mean_weights_[i] << " ";
    cout << "\n";

    AgeSizePtr age_size = categories->age_size(category);
    cout << "age_weight: ";
    unsigned age_spread = (categories->max_age(category) - categories->min_age(category)) + 1;
    for (unsigned i = 0; i < age_spread; ++i)
      cout << age_size->mean_weight(i + (*iterator)->min_age_) << " ";
    cout << "\n";

    cout << "age_size: ";
    for (unsigned i = 0; i < age_spread; ++i)
          cout << age_size->mean_size(i + (*iterator)->min_age_) << " ";
    cout << "\n";
  }

  cout << "*end" << endl;

}

} /* namespace reports */
} /* namespace isam */
