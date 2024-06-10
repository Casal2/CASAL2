/**
 * @file CGammaDiffCallback.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 17/04/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifdef USE_AUTODIFF
#ifdef USE_ADOLC
// headers
#include "Callback.h"

#include "../../../Estimates/Manager.h"
#include "../../../ObjectiveFunction/ObjectiveFunction.h"

// namespaces
namespace niwa {
namespace minimisers {
namespace adolc {

/**
 * Default Constructor
 */
CallBack::CallBack(shared_ptr<Model> model) : model_(model) {}

//**********************************************************************
// double CGammaDiffCallback::operator()(const vector<double>& Parameters)
// Operator() for Minimiser CallBack
//**********************************************************************
adouble CallBack::operator()(const vector<adouble>& Parameters) {
  LOG_MEDIUM() << "ADOL-C Casal2 Callback Called";
  // Update our Components with the New Parameters
  auto estimates = model_->managers()->estimate()->GetIsEstimated();

  if (Parameters.size() != estimates.size()) {
    LOG_CODE_ERROR() << "The number of enabled estimates does not match the number of test solution values";
  }

  for (unsigned i = 0; i < Parameters.size(); ++i) estimates[i]->set_value(Parameters[i]);

  model_->FullIteration();

  ObjectiveFunction& objective = model_->objective_function();
  objective.CalculateScore();

  return objective.score();
}

} /* namespace adolc */
}  // namespace minimisers
} /* namespace niwa */
#endif /* USE_ADOLC */
#endif /* USE_AUTODIFF */
