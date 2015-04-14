/**
 * @file ADOLC.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef USE_AUTODIFF
#ifdef USE_ADOLC

// headers
#include "ADOLC.h"

#include "ADOLC/Engine.h"
#include "ADOLC/Callback.h"
#include "Estimates/Manager.h"

namespace niwa {
namespace minimisers {

/**
 *
 */
ADOLC::ADOLC() {
  parameters_.Bind<int>(PARAM_MAX_ITERATIONS, &max_iterations_, "Maximum number of iterations", "", 1000);
  parameters_.Bind<int>(PARAM_MAX_EVALUATIONS, &max_evaluations_, "Maximum number of evaluations", "", 4000);
  parameters_.Bind<Double>(PARAM_TOLERANCE, &gradient_tolerance_, "Tolerance of the gradient for convergence", "", 0.02);
  parameters_.Bind<Double>(PARAM_STEP_SIZE, &step_size_, "Minimum Step-size before minimisation fails", "", 1e-7);
}

/**
 * Execute the minimiser to solve the model
 */
void ADOLC::Execute() {
  LOG_TRACE();
  // Variables
  adolc::CallBack  call_back;

  estimates::Manager& estimate_manager = estimates::Manager::Instance();

  vector<Double>  lower_bounds;
  vector<Double>  upper_bounds;
  vector<Double>  start_values;

  vector<EstimatePtr> estimates = estimate_manager.GetEnabled();
  for (EstimatePtr estimate : estimates) {
    if (!estimate->enabled())
      continue;

    lower_bounds.push_back(estimate->lower_bound());
    upper_bounds.push_back(estimate->upper_bound());
    start_values.push_back(estimate->GetTransformedValue());

    if (estimate->GetTransformedValue() < estimate->lower_bound()) {
      LOG_ERROR_P("When starting the GammDiff numerical_differences minimiser the starting value (" << estimate->GetTransformedValue() << ") for estimate "
          << estimate->parameter() << " was less than the lower bound (" << estimate->lower_bound() << ")");
    } else if (estimate->GetTransformedValue() > estimate->upper_bound()) {
      LOG_ERROR_P("When starting the GammDiff numerical_differences minimiser the starting value (" << estimate->GetTransformedValue() << ") for estimate "
          << estimate->parameter() << " was greater than the upper bound (" << estimate->upper_bound() << ")");
    }
  }

  int status = 0;
  adolc::Engine adolc;
  adolc.optimise(call_back,
      start_values, lower_bounds, upper_bounds,
      status, max_iterations_, max_evaluations_, gradient_tolerance_,
      hessian_,1,step_size_);
}

} /* namespace minimisers */
} /* namespace niwa */

#endif /* USE_ADOLC */
#endif /* USE_AUTODIFF */
