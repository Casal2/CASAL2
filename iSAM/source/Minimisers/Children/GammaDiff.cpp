//============================================================================
// Name        : GammaDiff.cpp
// Author      : S.Rasmussen
// Date        : 8/09/2008
// Copyright   : Copyright NIWA Science ©2008 - www.niwa.co.nz
// Description :
// $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
//============================================================================

// Local Headers
#include "GammaDiff.h"

#include "GammaDiff/Callback.h"
#include "GammaDiff/Engine.h"
#include "Estimates/Manager.h"

// namespaces
namespace isam {
namespace minimisers {

/**
 * Default constructor
 */
GammaDiff::GammaDiff() {
  parameters_.RegisterAllowed(PARAM_MAX_ITERATIONS);
  parameters_.RegisterAllowed(PARAM_MAX_EVALUATIONS);
  parameters_.RegisterAllowed(PARAM_TOLERANCE);
  parameters_.RegisterAllowed(PARAM_STEP_SIZE);
}

/**
 * Validate the minimiser
 */
void GammaDiff::Validate() {
  Minimiser::Validate();

  max_iterations_       = parameters_.Get(PARAM_MAX_ITERATIONS).GetValue<int>(1000);
  max_evaluations_      = parameters_.Get(PARAM_MAX_EVALUATIONS).GetValue<int>(4000);
  gradient_tolerance_   = parameters_.Get(PARAM_TOLERANCE).GetValue<double>(0.02);
  step_size_            = parameters_.Get(PARAM_STEP_SIZE).GetValue<double>(1e-7);
}

/**
 * Execute the minimiser to solve the model
 */
void GammaDiff::Execute() {
  // Variables
  gammadiff::CallBack  call_back;

  estimates::Manager& estimate_manager = estimates::Manager::Instance();

  vector<double>  lower_bounds;
  vector<double>  upper_bounds;
  vector<double>  start_values;

  vector<EstimatePtr> estimates = estimate_manager.GetObjects();
  for (EstimatePtr estimate : estimates) {
    if (!estimate->enabled())
      continue;

    lower_bounds.push_back(estimate->lower_bound());
    upper_bounds.push_back(estimate->upper_bound());
    start_values.push_back(estimate->value());

    if (estimate->value() < estimate->lower_bound()) {
      LOG_ERROR("When starting the DESolver minimiser the starting value (" << estimate->value() << ") for estimate "
          << estimate->parameter() << " was less than the lower bound (" << estimate->lower_bound() << ")");
    } else if (estimate->value() > estimate->upper_bound()) {
      LOG_ERROR("When starting the DESolver minimiser the starting value (" << estimate->value() << ") for estimate "
          << estimate->parameter() << " was greater than the upper bound (" << estimate->upper_bound() << ")");
    }
  }

  double **pHessian = 0;

  int iEstimateCount = (int)estimates.size();
  pHessian = new double*[iEstimateCount];
  for (int i = 0; i < iEstimateCount; ++i)
    pHessian[i] = new double[iEstimateCount];

  for (int i = 0; i < iEstimateCount; ++i)
    for (int j = 0; j < iEstimateCount; ++j)
      pHessian[i][j] = 0.0;

  int status = 0;
  gammadiff::Engine clGammaDiff;
  clGammaDiff.optimise_finite_differences(call_back,
      start_values, lower_bounds, upper_bounds,
      status, max_iterations_, max_evaluations_, gradient_tolerance_,
      pHessian,1,step_size_);
}

} /* namespace minimisers */
} /* namespace isam */
