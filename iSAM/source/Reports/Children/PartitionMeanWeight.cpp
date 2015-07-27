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

#include "AgeLengths/Manager.h"
#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Partition/Partition.h"
#include "Partition/Accessors/All.h"
#include "LengthWeights/Manager.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * default constructor
 */
PartitionMeanWeight::PartitionMeanWeight() {
  run_mode_    = RunMode::kBasic;
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "Time Step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years", "", true);
}

/**
 * Execute method
 */
void PartitionMeanWeight::DoExecute() {

  CategoriesPtr categories = Categories::Instance();
  niwa::partition::accessors::All all_view;

  /*
  cache_ << "*" << this->label() << "\n";

  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    (*iterator)->UpdateMeanWeightData();
    (*iterator)->UpdateMeanLengthData();

    string category = (*iterator)->name_;
    cache_ << "category: " << category << "\n";

    cache_ << "mean_weights:\n";
    unsigned year = Model::Instance()->current_year();
    cache_ << year << ": ";

    for (unsigned age = (*iterator)->min_age_; age <= (*iterator)->max_age_; ++age)
      cache_ << (*iterator)->weight_per_[age] << " ";

    cache_ << "\n";


    cache_ << "age_lengths:\n";
    cache_ << year << ": ";

    for (unsigned age = (*iterator)->min_age_; age <= (*iterator)->max_age_; ++age)
      cache_ << (*iterator)->length_per_[age] << " ";

    cache_ << "\n";
  }

  cache_ << "*end" << endl;
  */

  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";

  for (auto iterator = all_view.Begin(); iterator != all_view.End(); ++iterator) {
    (*iterator)->UpdateMeanWeightData();
    (*iterator)->UpdateMeanLengthData();

    string category = (*iterator)->name_;
    cache_ << category << " " << REPORT_R_LIST << "\n";

    cache_ << "mean_weights " << REPORT_R_LIST << "\n";
    unsigned year = Model::Instance()->current_year();
    cache_ << year << ": ";

    for (unsigned age = (*iterator)->min_age_; age <= (*iterator)->max_age_; ++age)
      cache_ << (*iterator)->weight_per_[age] << " ";
    cache_<<"\n";

    cache_ << REPORT_R_LIST_END <<"\n";


    cache_ << "age_lengths " << REPORT_R_LIST << "\n";
    cache_ << year << ": ";

    for (unsigned age = (*iterator)->min_age_; age <= (*iterator)->max_age_; ++age)
      cache_ << (*iterator)->length_per_[age] << " ";
    cache_<<"\n";

    cache_ << REPORT_R_LIST_END <<"\n";

    cache_ << REPORT_R_LIST_END <<"\n";
  }


  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
