//============================================================================
// Name        : GammaDiff.cpp
// Author      : S.Rasmussen
// Date        : 8/09/2008
// Copyright   : Copyright NIWA Science ©2008 - www.niwa.co.nz
// Description :
// $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
//============================================================================
#ifndef USE_AUTODIFF
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
  parameters_.Bind<int>(PARAM_MAX_ITERATIONS, &max_iterations_, "Maximum number of iterations", "", 1000);
  parameters_.Bind<int>(PARAM_MAX_EVALUATIONS, &max_evaluations_, "Maximum number of evaluations", "", 4000);
  parameters_.Bind<double>(PARAM_TOLERANCE, &gradient_tolerance_, "Tolerance of the gradient for convergence", "", 0.02);
  parameters_.Bind<double>(PARAM_STEP_SIZE, &step_size_, "Minimum Step-size before minimisation fails", "", 1e-7);
}

/**
 * Execute the minimiser to solve the model
 */
void GammaDiff::Execute() {
  LOG_TRACE();
  // Variables
  gammadiff::CallBack  call_back;

  estimates::Manager& estimate_manager = estimates::Manager::Instance();

  vector<double>  lower_bounds;
  vector<double>  upper_bounds;
  vector<double>  start_values;

  vector<EstimatePtr> estimates = estimate_manager.GetEnabled();
  for (EstimatePtr estimate : estimates) {
    if (!estimate->enabled())
      continue;

    lower_bounds.push_back((double)estimate->lower_bound());
    upper_bounds.push_back((double)estimate->upper_bound());
    start_values.push_back((double)estimate->GetTransformedValue());

    if (estimate->GetTransformedValue() < estimate->lower_bound()) {
      LOG_ERROR("When starting the GammDiff numerical_differences minimiser the starting value (" << estimate->GetTransformedValue() << ") for estimate "
          << estimate->parameter() << " was less than the lower bound (" << estimate->lower_bound() << ")");
    } else if (estimate->GetTransformedValue() > estimate->upper_bound()) {
      LOG_ERROR("When starting the GammDiff numerical_differences minimiser the starting value (" << estimate->GetTransformedValue() << ") for estimate "
          << estimate->parameter() << " was greater than the upper bound (" << estimate->upper_bound() << ")");
    }
  }

  int status = 0;
  gammadiff::Engine clGammaDiff;
  clGammaDiff.optimise_finite_differences(call_back,
      start_values, lower_bounds, upper_bounds,
      status, max_iterations_, max_evaluations_, gradient_tolerance_,
      hessian_,1,step_size_);
}

} /* namespace minimisers */
} /* namespace isam */
#endif
