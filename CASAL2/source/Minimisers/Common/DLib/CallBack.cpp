
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
#ifndef _MSC_VER

// headers
#include "CallBack.h"

#include "../../../Estimates/Manager.h"
#include "../../../EstimateTransformations/Manager.h"
#include "../../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../../Utilities/Math.h"

// namespaces
namespace niwa {
namespace minimisers {
namespace dlib {

/**
 * Default constructor
 */
Callback::Callback(shared_ptr<Model> model) : model_(model) {
}

/**
 * Operator ()
 */
Double Callback::operator()(const ::dlib::matrix<double, 0, 1>& Parameters) const {
  // Update our Components with the New Parameters
  vector<Estimate*> estimates = model_->managers()->estimate()->GetIsEstimated();

  if (Parameters.size() != (int)estimates.size()) {
    LOG_CODE_ERROR() << "The number of enabled estimates does not match the number of test solution values";
  }

  double penalty = 0;
  for (int i = 0; i < Parameters.size(); ++i) {
    // Note, we unscale here because DLib doesn't have concept of bounds. This allows us to
    // add a penalty if the values exceed the bounds.
    Double value = utilities::math::unscale_value(Parameters(i), penalty, estimates[i]->lower_bound(), estimates[i]->upper_bound());
    estimates[i]->set_value(value);
  }

  model_->managers()->estimate_transformation()->RestoreEstimates();
  model_->FullIteration();
  LOG_MEDIUM() << "Iteration Complete";
  ObjectiveFunction& objective = model_->objective_function();
  objective.CalculateScore();

  model_->managers()->estimate_transformation()->TransformEstimates();
  double score = objective.score() + penalty;
  return score;
}

} /* namespace dlib */
} /* namespace minimisers */
} /* namespace niwa */
#endif
#endif /* USE_AUTODIFF */
