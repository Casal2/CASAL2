//============================================================================
// Name        : GammaDiff.cpp
// Author      : S.Rasmussen
// Date        : 8/09/2008
// Copyright   : Copyright NIWA Science �2008 - www.niwa.co.nz
// Description :
// $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
//============================================================================
#ifndef USE_AUTODIFF
// Local Headers
#include "GammaDiff.h"

#include "GammaDiff/Callback.h"
#include "GammaDiff/Engine.h"
#include "Estimates/Manager.h"
#include "EstimateTransformations/Manager.h"

// namespaces
namespace niwa {
namespace minimisers {

/**
 * Default constructor
 */
GammaDiff::GammaDiff(Model* model) : Minimiser(model) {
  parameters_.Bind<int>(PARAM_MAX_ITERATIONS, &max_iterations_, "Maximum number of iterations", "", 1000);
  parameters_.Bind<int>(PARAM_MAX_EVALUATIONS, &max_evaluations_, "Maximum number of evaluations", "", 4000);
  parameters_.Bind<Double>(PARAM_TOLERANCE, &gradient_tolerance_, "Tolerance of the gradient for convergence", "", 0.02);
  parameters_.Bind<Double>(PARAM_STEP_SIZE, &step_size_, "Minimum Step-size before minimisation fails", "", 1e-7);
}

/**
 * Execute the minimiser to solve the model
 */
void GammaDiff::Execute() {
  LOG_TRACE();
  // Variables
  LOG_FINE() << "model_: " << model_;

  gammadiff::CallBack  call_back(model_);
  estimates::Manager* estimate_manager = model_->managers().estimate();
  LOG_FINE() << "estimate_manager: " << estimate_manager;

  vector<double>  lower_bounds;
  vector<double>  upper_bounds;
  vector<double>  start_values;

  model_->managers().estimate_transformation()->TransformEstimates();
  vector<Estimate*> estimates = estimate_manager->GetIsEstimated();
  LOG_FINE() << "estimates.size(): " << estimates.size();
  for (Estimate* estimate : estimates) {
    if (!estimate->estimated())
      continue;

    LOG_FINE() << "Estimate: " << estimate;
    LOG_FINE() << "transformed value: " << estimate->value();
    LOG_FINE() << "Parameter: " << estimate->parameter();

    lower_bounds.push_back((double)estimate->lower_bound());
    upper_bounds.push_back((double)estimate->upper_bound());
    start_values.push_back((double)estimate->value());

    if (estimate->value() < estimate->lower_bound()) {
      LOG_FATAL() << "When starting the GammDiff gamma_diff minimiser the starting value (" << estimate->value() << ") for estimate "
          << estimate->parameter() << " was less than the lower bound (" << estimate->lower_bound() << ")";
    } else if (estimate->value() > estimate->upper_bound()) {
      LOG_FATAL() << "When starting the GammDiff gamma_diff minimiser the starting value (" << estimate->value() << ") for estimate "
          << estimate->parameter() << " was greater than the upper bound (" << estimate->upper_bound() << ")";
    }
  }

  LOG_FINE() << "Launching minimiser";
  int status = 0;
  gammadiff::Engine clGammaDiff;
  clGammaDiff.optimise_finite_differences(call_back,
      start_values, lower_bounds, upper_bounds,
      status, max_iterations_, max_evaluations_, gradient_tolerance_,
      hessian_,1,step_size_);

  model_->managers().estimate_transformation()->RestoreEstimates();
}

} /* namespace minimisers */
} /* namespace niwa */
#endif
