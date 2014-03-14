/**
 * @file Callback.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 14/03/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */

// headers
#include "Callback.h"

#include "Estimates/Manager.h"
#include "ObjectiveFunction/ObjectiveFunction.h"

// namespaces
namespace isam {
namespace minimisers {
namespace dlib {

/**
 *
 */
Callback::Callback() {
  model_ = Model::Instance();
}

/**
 *
 */
double Callback::operator()(const ::dlib::matrix<double, 0, 1>& Parameters) const {
  // Update our Components with the New Parameters
  vector<EstimatePtr> estimates = estimates::Manager::Instance().GetEnabled();

  if (Parameters.size() != (int)estimates.size()) {
    LOG_CODE_ERROR("The number of enabled estimates does not match the number of test solution values");
  }

  for (int i = 0; i < Parameters.size(); ++i)
    estimates[i]->set_value(Parameters(i));

  ObjectiveFunction& objective = ObjectiveFunction::Instance();

  model_->FullIteration();

  objective.CalculateScore();
  return objective.score();
}

} /* namespace dlib */
} /* namespace minimisers */
} /* namespace isam */
