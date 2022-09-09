/**
 * @file CategoryInfo.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "CategoryInfo.h"

#include "../../Categories/Categories.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
CategoryInfo::CategoryInfo() {
  run_mode_    = (RunMode::Type)(RunMode::kBasic | RunMode::kEstimation | RunMode::kProfiling);
  model_state_ = State::kFinalise;
}

/**
 * Execute the report
 */
void CategoryInfo::DoExecute(shared_ptr<Model> model) {
  auto           categories = model->categories();
  vector<string> names      = categories->category_names();

  cache_ << ReportHeader(type_, label_, format_);
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

void CategoryInfo::DoPrepareTabular(shared_ptr<Model> model) {
  LOG_INFO() << "Tabular mode for reports of type " << PARAM_CATEGORY_INFO << " has not been implemented";
}

} /* namespace reports */
} /* namespace niwa */
