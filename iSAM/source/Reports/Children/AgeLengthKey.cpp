/**
 * @file AgeLengthKey.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 18/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "AgeLengthKey.h"

#include "Categories/Categories.h"
#include "Model/Model.h"
#include "Partition/Accessors/Categories.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
AgeLengthKey::AgeLengthKey() {
  run_mode_    = RunMode::kBasic;
  model_state_ = State::kExecute;

  parameters_.Bind<string>(PARAM_TIME_STEP, &time_step_, "Time Step label", "", "");
  parameters_.Bind<unsigned>(PARAM_YEARS, &years_, "Years", "", true);
}

/**
 *
 */
void AgeLengthKey::DoExecute() {
  partition::accessors::Categories length_partition;
  partition::accessors::Categories age_partition;

  /*
  cache_ << "*age_length_key summary report: " << label_ << "\n";
  length_partition.Init(Categories::Instance()->category_names());
  age_partition.Init(Categories::Instance()->category_names());

  auto l_iter = length_partition.begin();
  auto a_iter = age_partition.begin();
  for (; l_iter != length_partition.end(); ++l_iter, ++a_iter) {
    cache_ << "category: " << (*a_iter)->name_ << "\n";
    cache_ << "age_data: ";
    for (Double value : (*a_iter)->data_)
        cache_ << value << " ";
    cache_ << "\n";
    cache_ << "age/length matrix:\n";

    vector<Double> length_bins = Model::Instance()->length_bins();
    cache_ << "    age ";
    for (Double length_bin : length_bins)
      cache_ << length_bin << " ";
    cache_ << "\n";

    (*l_iter)->ConvertAgeDataToLength();

    for (auto iter : (*l_iter)->length_data_) {
      cache_ << "    " << iter.first << " ";
      for (auto x : iter.second)
        cache_ << x.second << " ";
      cache_ << "\n";
    }
    cache_ << "collapsed_age_data: ";
    for (Double value : (*a_iter)->data_)
        cache_ << value << " ";
    cache_ << "\n";
  }
*/
  cache_ << "*" << label_ << " " << "("<< type_ << ")"<<"\n";
  length_partition.Init(Categories::Instance()->category_names());
  age_partition.Init(Categories::Instance()->category_names());

  auto l_iter = length_partition.begin();
  auto a_iter = age_partition.begin();
  for (; l_iter != length_partition.end(); ++l_iter, ++a_iter) {
    cache_ << (*a_iter)->name_ << " " << REPORT_R_LIST <<"\n";
    cache_ << "age_data: ";
    for (Double value : (*a_iter)->data_)
        cache_ << value << " ";
    cache_ << "\n";
    cache_ << "age_length_key " << REPORT_R_MATRIX <<"\n";

    vector<Double> length_bins = Model::Instance()->length_bins();
    cache_ << "    age ";
    for (Double length_bin : length_bins)
      cache_ << length_bin << " ";
    cache_ << "\n";

    (*l_iter)->ConvertAgeDataToLength();

    for (auto iter : (*l_iter)->length_data_) {
      cache_ << "    " << iter.first << " ";
      for (auto x : iter.second)
        cache_ << x.second << " ";
      cache_ << "\n";
    }
    cache_ << "collapsed_age_data: ";
    for (Double value : (*a_iter)->data_)
        cache_ << value << " ";
    cache_ << "\n";
    cache_ << REPORT_R_LIST_END <<"\n";
  }
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
