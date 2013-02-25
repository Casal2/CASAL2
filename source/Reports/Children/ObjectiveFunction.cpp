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
namespace isam {
namespace reports {

/**
 * Default constructor
 */
ObjectiveFunction::ObjectiveFunction() {
  model_state_ = State::kFinalise;
}

/**
 * Execute the report
 */
void ObjectiveFunction::Execute() {

  // Header
  cout << CONFIG_ARRAY_START << label_ << CONFIG_ARRAY_END << "\n";
  cout << PARAM_REPORT << "." << PARAM_TYPE << CONFIG_RATIO_SEPARATOR << " " << parameters_.Get(PARAM_TYPE).GetValue<string>() << "\n";


  ::isam::ObjectiveFunction obj_function = isam::ObjectiveFunction::Instance();

  const vector<objective::Score>& score_list = obj_function.score_list();
  for (objective::Score score : score_list) {
    cout << score.label_ << ": " << score.score_ << "\n";
  }

  cout << PARAM_TOTAL_SCORE << ": " << obj_function.score() << "\n";
  cout << CONFIG_END_REPORT << "\n" << endl;
}

} /* namespace reports */
} /* namespace isam */
