/**
 * @file CategoryList.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 7/07/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 */

// headers
#include "CategoryList.h"

#include "Common/Categories/Categories.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
CategoryList::CategoryList(Model* model) : Report(model) {
  run_mode_    = RunMode::kBasic;
  model_state_ = State::kFinalise;
}

/**
 * Execute the report
 */
void CategoryList::DoExecute() {
  cache_ << "*category_list: " << label_ << "\n";

  auto categories = model_->categories();

  vector<string> names = categories->category_names();
  for(string name : names) {
    cache_ << name << "\n";
  }
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
