/**
 * @file CGammaDiffCallback.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 17/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef USE_AUTODIFF
#ifdef USE_ADOLC
// headers
#include "Callback.h"

#include "Estimates/Manager.h"
#include "EstimateTransformations/Manager.h"
#include "ObjectiveFunction/ObjectiveFunction.h"

// namespaces
namespace niwa {
namespace minimisers {
namespace adolc {

/**
 * Default Constructor
 */
CallBack::CallBack(Model* model) : model_(model) {
}

//**********************************************************************
// double CGammaDiffCallback::operator()(const vector<double>& Parameters)
// Operatior() for Minimiser CallBack
//**********************************************************************
adouble CallBack::operator()(const vector<adouble>& Parameters) {

  // Update our Components with the New Parameters
  auto estimates = model_->managers().estimate()->GetIsEstimated();

  if (Parameters.size() != estimates.size()) {
    LOG_CODE_ERROR() << "The number of enabled estimates does not match the number of test solution values";
  }

  for (unsigned i = 0; i < Parameters.size(); ++i)
    estimates[i]->set_value(AS_VALUE(Parameters[i]));

  model_->managers().estimate_transformation()->RestoreEstimates();
  model_->FullIteration();

  ObjectiveFunction& objective = model_->objective_function();
  objective.CalculateScore();

  model_->managers().estimate_transformation()->TransformEstimates();
  return objective.score();
}

} /* namespace adolc */
} /* namespace minimiser */
} /* namespace niwa */
#endif /* USE_ADOLC */
#endif /* USE_AUTODIFF */
