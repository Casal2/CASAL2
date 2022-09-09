/**
 * @file CategoryList.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 7/07/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 */

// headers
#include "CategoryList.h"

#include "../../Categories/Categories.h"

// namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
CategoryList::CategoryList() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = State::kFinalise;
}

/**
 * Execute the report
 */
void CategoryList::DoExecute(shared_ptr<Model> model) {
  cache_ << ReportHeader(type_, label_, format_);
  auto categories = model->categories();

  vector<string> names = categories->category_names();
  for (string name : names) {
    cache_ << name << " " << REPORT_R_LIST << REPORT_EOL;
    cache_ << "min_age: " << categories->min_age(name) << REPORT_EOL;
    cache_ << "max_age: " << categories->max_age(name) << REPORT_EOL;

    vector<unsigned> years = categories->years(name);
    cache_ << "years: ";
    for (unsigned year : years) cache_ << year << " ";
    cache_ << REPORT_EOL;
    cache_ << REPORT_R_LIST_END << REPORT_EOL;

  }
  ready_for_writing_ = true;
}

void CategoryList::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_CATEGORY_LIST << " has not been implemented";
}

} /* namespace reports */
} /* namespace niwa */
