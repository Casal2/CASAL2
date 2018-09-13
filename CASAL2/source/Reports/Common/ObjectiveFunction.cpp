/**
 * @file ObjectiveFunction.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "ObjectiveFunction.h"

#include "ObjectiveFunction/ObjectiveFunction.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 * Default constructor
 */
ObjectiveFunction::ObjectiveFunction(Model* model) : Report(model) {
  model_state_ = State::kIterationComplete;
  run_mode_    = (RunMode::Type)(RunMode::kEstimation | RunMode::kBasic | RunMode::kProjection| RunMode::kProfiling);
}

/**
 * Execute the report
 */
void ObjectiveFunction::DoExecute() {
  cache_ << "*"<< type_ << "[" << label_ << "]" << "\n";
  cache_ <<"values " << REPORT_R_VECTOR <<"\n";

  ::niwa::ObjectiveFunction& obj_function = model_->objective_function();

  const vector<objective::Score>& score_list = obj_function.score_list();
  for (objective::Score score : score_list) {
    cache_ << score.label_ << " " << AS_DOUBLE(score.score_) << "\n";
  }

  cache_ << PARAM_TOTAL_SCORE << " " << AS_DOUBLE(obj_function.score()) << "\n";
  ready_for_writing_ = true;
}


} /* namespace reports */
} /* namespace niwa */
