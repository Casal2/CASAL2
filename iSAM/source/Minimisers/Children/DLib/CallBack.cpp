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
#include "CallBack.h"

#include "Estimates/Manager.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Utilities/Math.h"

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

  double penalty = 0;
  for (int i = 0; i < Parameters.size(); ++i) {
    double value = utilities::math::unScaleValue(Parameters(i), penalty, estimates[i]->lower_bound(), estimates[i]->upper_bound());
    estimates[i]->SetTransformedValue(value);
  }

  ObjectiveFunction& objective = ObjectiveFunction::Instance();

  model_->FullIteration();

  objective.CalculateScore();
  return objective.score() + penalty;
}

} /* namespace dlib */
} /* namespace minimisers */
} /* namespace isam */
