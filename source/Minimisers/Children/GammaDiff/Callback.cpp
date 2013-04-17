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

// headers
#include "Callback.h"

#include "Estimates/Manager.h"
#include "ObjectiveFunction/ObjectiveFunction.h"

// namespaces
namespace isam {
namespace minimisers {
namespace gammadiff {

/**
 * Default Constructor
 */
CallBack::CallBack() {
  model_ = Model::Instance();
}

//**********************************************************************
// double CGammaDiffCallback::operator()(const vector<double>& Parameters)
// Operatior() for Minimiser CallBack
//**********************************************************************
double CallBack::operator()(const vector<double>& Parameters) {

  // Update our Components with the New Parameters
  vector<EstimatePtr> estimates = estimates::Manager::Instance().GetEnabled();

  if (Parameters.size() != estimates.size()) {
    LOG_CODE_ERROR("The number of enabled estimates does not match the number of test solution values");
  }

  for (unsigned i = 0; i < Parameters.size(); ++i)
    estimates[i]->set_value(Parameters[i]);

  ObjectiveFunction& objective = ObjectiveFunction::Instance();

  model_->FullIteration();

  objective.CalculateScore();
  return objective.score();
}

} /* namespace gammadiff */
} /* namespace minimiser */
} /* namespace isam */
