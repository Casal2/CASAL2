
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
#ifndef USE_AUTODIFF
// headers
#include "Callback.h"

#include "../../../Estimates/Manager.h"
#include "../../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../../ThreadPool/Thread.h"
#include "../../../ThreadPool/ThreadPool.h"

// namespaces
namespace niwa {
namespace minimisers {
namespace deltadiff {

/**
 * Default Constructor
 */
CallBack::CallBack(shared_ptr<ThreadPool> thread_pool) : thread_pool_(thread_pool) {}

//**********************************************************************
// double CGammaDiffCallback::operator()(const vector<double>& Parameters)
// Operator() for Minimiser CallBack
//**********************************************************************
double CallBack::operator()(const vector<double>& Parameters) {
  auto model_ = thread_pool_->threads()[0]->model();

  // Update our Components with the New Parameters
  LOG_FINE() << "model_: " << model_;
  vector<Estimate*> estimates = model_->managers()->estimate()->GetIsEstimated();

  if (Parameters.size() != estimates.size()) {
    LOG_CODE_ERROR() << "The number of enabled estimates does not match the number of test solution values";
  }

  for (unsigned i = 0; i < Parameters.size(); ++i) estimates[i]->set_value(Parameters[i]);

  model_->FullIteration();

  ObjectiveFunction& objective = model_->objective_function();
  objective.CalculateScore();


  ++count_;

  return objective.score();
}

/**
 *
 */
void CallBack::operator()(const vector<vector<double>>& Parameters, vector<double>& scores) {
  LOG_MEDIUM() << "Calls since last gradient: " << count_;
  count_ = 0;
  thread_pool_->RunCandidates(Parameters, scores);
}

} /* namespace deltadiff */
}  // namespace minimisers
} /* namespace niwa */
#endif
