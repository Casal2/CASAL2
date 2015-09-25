/**
 * @file Callback.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 14/03/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifndef USE_AUTODIFF

// headers
#include "CallBack.h"

#include "Estimates/Manager.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Utilities/Math.h"

// namespaces
namespace niwa {
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
Double Callback::operator()(const ::dlib::matrix<double, 0, 1>& Parameters) const {
  // Update our Components with the New Parameters
  vector<Estimate*> estimates = estimates::Manager::Instance().GetEnabled();

  if (Parameters.size() != (int)estimates.size()) {
    LOG_CODE_ERROR() << "The number of enabled estimates does not match the number of test solution values";
  }

  double penalty = 0;
  for (int i = 0; i < Parameters.size(); ++i) {
    Double value = utilities::math::unscale_value(Parameters(i), penalty, estimates[i]->lower_bound(), estimates[i]->upper_bound());
    estimates[i]->SetTransformedValue(value);
  }

  ObjectiveFunction& objective = ObjectiveFunction::Instance();

  model_->FullIteration();

  objective.CalculateScore();
  return objective.score() + penalty;
}

} /* namespace dlib */
} /* namespace minimisers */
} /* namespace niwa */
#endif /* USE_AUTODIFF */
