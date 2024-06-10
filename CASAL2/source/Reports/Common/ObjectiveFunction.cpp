/**
 * @file ObjectiveFunction.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/02/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "ObjectiveFunction.h"

#include "../../ObjectiveFunction/ObjectiveFunction.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
ObjectiveFunction::ObjectiveFunction() {
  model_state_ = State::kIterationComplete;
  run_mode_    = (RunMode::Type)(RunMode::kEstimation | RunMode::kBasic | RunMode::kProfiling);
}

/**
 *
 * Execute the report
 */
void ObjectiveFunction::DoExecute(shared_ptr<Model> model) {
  LOG_MEDIUM() << "Objective function report DoExecute";
  if (!model->is_primary_thread_model() && model->run_mode() == RunMode::kBasic)
    return;
  if (!model->is_primary_thread_model() && model->run_mode() == RunMode::kEstimation)
    return;

  cache_ << ReportHeader(type_, label_, format_);
  cache_ << "values " << REPORT_R_VECTOR << REPORT_EOL;

  if (model == nullptr)
    LOG_CODE_ERROR() << "model_ == nullptr";
  ::niwa::ObjectiveFunction& obj_function = model->objective_function();

  const vector<objective::Score>& score_list = obj_function.score_list();
  for (objective::Score score : score_list) {
    cache_ << score.label_ << " " << AS_DOUBLE(score.score_) << REPORT_EOL;
  }
  cache_ << PARAM_TOTAL_SCORE << " " << AS_DOUBLE(obj_function.score()) << REPORT_EOL;
  ready_for_writing_ = true;
}

void ObjectiveFunction::DoPrepareTabular(shared_ptr<Model> model) {
  if (model->is_primary_thread_model()) {
    if (model == nullptr)
      LOG_CODE_ERROR() << "model_ == nullptr";
    cache_ << ReportHeader(type_, label_, format_);
    cache_ << "values " << REPORT_R_DATAFRAME << REPORT_EOL;
  }
}

void ObjectiveFunction::DoExecuteTabular(shared_ptr<Model> model) {
  if (model->is_primary_thread_model()) {
    if (model == nullptr)
      LOG_CODE_ERROR() << "model_ == nullptr";
    ::niwa::ObjectiveFunction&      obj_function = model->objective_function();
    const vector<objective::Score>& score_list   = obj_function.score_list();
    if (first_run_) {
      first_run_ = false;
      for (objective::Score score : score_list) {
        cache_ << PARAM_OBJECTIVE_FUNCTION << "[" << score.label_ << "] ";
      }
      cache_ << REPORT_EOL;
    }
    for (objective::Score score : score_list) {
      cache_ << AS_DOUBLE(score.score_) << " ";
    }
    cache_ << REPORT_EOL;
  }
}
void ObjectiveFunction::DoFinaliseTabular(shared_ptr<Model> model) {
  ready_for_writing_ = true;
}

} /* namespace reports */
} /* namespace niwa */
