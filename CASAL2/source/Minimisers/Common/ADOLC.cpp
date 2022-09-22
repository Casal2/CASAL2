/**
 * @file ADOLC.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */
#ifdef USE_AUTODIFF
#ifdef USE_ADOLC

// headers
#include "ADOLC.h"

#include "../../Estimates/Manager.h"
#include "ADOLC/Callback.h"
#include "ADOLC/Engine.h"

namespace niwa {
namespace minimisers {

/**
 *
 */
ADOLC::ADOLC(shared_ptr<Model> model) : Minimiser(model) {
  parameters_.Bind<int>(PARAM_MAX_ITERATIONS, &max_iterations_, "The maximum number of iterations", "", 1000)->set_lower_bound(1);
  parameters_.Bind<int>(PARAM_MAX_EVALUATIONS, &max_evaluations_, "The maximum number of evaluations", "", 4000)->set_lower_bound(1);
  parameters_.Bind<double>(PARAM_TOLERANCE, &gradient_tolerance_, "The tolerance of the gradient for convergence", "", DEFAULT_CONVERGENCE)->set_lower_bound(0.0, false);
  parameters_.Bind<double>(PARAM_STEP_SIZE, &step_size_, "The minimum step size before minimisation fails", "", 1e-7)->set_lower_bound(0.0, false);
  parameters_
      .Bind<string>(PARAM_PARAMETER_TRANSFORMATION, &parameter_transformation_, "The choice of parametrisation used to scale the parameters for ADOLC", "", PARAM_SIN_TRANSFORM)
      ->set_allowed_values({PARAM_SIN_TRANSFORM, PARAM_TAN_TRANSFORM});
}

/**
 * Execute the minimiser
 */
void ADOLC::Execute() {
  LOG_TRACE();
  // Variables
  adolc::CallBack call_back(model_);

  auto estimate_manager = model_->managers()->estimate();

  if (parameter_transformation_ == PARAM_TAN_TRANSFORM) {
    use_tan_transform = true;
  } else {
    use_tan_transform = false;
  }
  vector<double> lower_bounds;
  vector<double> upper_bounds;
  vector<Double> start_values;

  LOG_INFO() << "Estimation with the " << PARAM_ADOLC << " minimiser";

  auto estimates = estimate_manager->GetIsEstimated();
  LOG_MEDIUM() << "estimated parameters";
  for (auto estimate : estimates) {
    LOG_MEDIUM() << estimate->parameter() << " value = " << estimate->value();
    lower_bounds.push_back(AS_DOUBLE(estimate->lower_bound()));
    upper_bounds.push_back(AS_DOUBLE(estimate->upper_bound()));
    start_values.push_back(estimate->value());

    if (estimate->value() < estimate->lower_bound()) {
      LOG_ERROR() << "The starting value for estimate " << estimate->parameter() << " (" << estimate->value() << ") was less than the lower bound (" << estimate->lower_bound()
                  << ")";
    } else if (estimate->value() > estimate->upper_bound()) {
      LOG_ERROR() << "The starting value for estimate " << estimate->parameter() << " (" << estimate->value() << ") was greater than the upper bound (" << estimate->upper_bound()
                  << ")";
    }
  }

  int           status = 0;
  adolc::Engine adolc;
  adolc.optimise(call_back, start_values, lower_bounds, upper_bounds, status, max_iterations_, max_evaluations_, gradient_tolerance_, hessian_, 1, step_size_, use_tan_transform);
  // Note C.M
  // The convergence check is done at ADOLC/Engine line 2013
  // But the convergence_ gets + 2 at line 297.
  // I have kept the result consistent with line 2013, but at a +2 in the following case: so that it is consistent.
  switch (adolc.get_convergence_status()) {
    case -3 + 2:
      result_ = MinimiserResult::kUnclearConvergence;
      break;
    case -2 + 2:
      result_ = MinimiserResult::kTooManyIterations;
      break;
    case -1 + 2:
      result_ = MinimiserResult::kSuccess;
      break;
    default:
      result_ = MinimiserResult::kError;
      break;
  }
}

} /* namespace minimisers */
} /* namespace niwa */

#endif /* USE_ADOLC */
#endif /* USE_AUTODIFF */
