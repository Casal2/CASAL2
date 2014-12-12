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
#include "Model/Model.h"
#include "Partition/Partition.h"
#include "Partition/Accessors/All.h"
#include "SizeWeights/Manager.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * default constructor
 */
PartitionMeanWeight::PartitionMeanWeight() {
  run_mode_    = RunMode::kBasic;
  model_state_ = State::kExecute;
}

/**
 * Execute method
 */
void PartitionMeanWeight::DoExecute() {
  cache_ << "*" << this->label() << "\n";

  CategoriesPtr categories = Categories::Instance();
  niwa::partition::accessors::All all_view;

  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    string category = (*iterator)->name_;
    cache_ << "category: " << category << "\n";

    cache_ << "mean_weights:\n";
    unsigned year = Model::Instance()->current_year();
    cache_ << year << ": ";

    for (unsigned age = (*iterator)->min_age_; age <= (*iterator)->max_age_; ++age)
      cache_ << (*iterator)->age_size_weight_->mean_weight(year, age) << " ";

    cache_ << "\n";


    cache_ << "age_sizes:\n";
    cache_ << year << ": ";

    for (unsigned age = (*iterator)->min_age_; age <= (*iterator)->max_age_; ++age)
      cache_ << (*iterator)->age_size_weight_->mean_size(year, age) << " ";

    cache_ << "\n";
  }

  cache_ << "*end" << endl;
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
