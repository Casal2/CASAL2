
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

#include "../../Estimates/Manager.h"
#include "../../Minimisers/Common/GammaDiff/Callback.h"
#include "../../Minimisers/Common/GammaDiff/Engine.h"

// namespaces
namespace niwa {
namespace minimisers {

/**
 * Default constructor
 */
GammaDiff::GammaDiff(shared_ptr<Model> model) : Minimiser(model) {
  parameters_.Bind<int>(PARAM_MAX_ITERATIONS, &max_iterations_, "The maximum number of iterations", "", 1000)->set_lower_bound(1);
  parameters_.Bind<int>(PARAM_MAX_EVALUATIONS, &max_evaluations_, "The maximum number of evaluations", "", 4000)->set_lower_bound(1);
  parameters_.Bind<double>(PARAM_TOLERANCE, &gradient_tolerance_, "The tolerance of the gradient for convergence", "", DEFAULT_CONVERGENCE)->set_lower_bound(0.0, false);
  parameters_.Bind<double>(PARAM_STEP_SIZE, &step_size_, "The minimum step size before minimisation fails", "", 1e-7)->set_lower_bound(0.0, false);
}

/**
 * Execute the minimiser
 */
void GammaDiff::Execute() {
  LOG_TRACE();
  // Variables
  LOG_FINE() << "model_: " << model_;

  LOG_INFO() << "Estimation with the " << PARAM_GAMMADIFF << " minimiser";

  gammadiff::CallBack call_back(model_);
  estimates::Manager* estimate_manager = model_->managers()->estimate();
  LOG_FINE() << "estimate_manager: " << estimate_manager;

  vector<double> lower_bounds;
  vector<double> upper_bounds;
  vector<double> start_values;

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
      LOG_ERROR() << "The starting value for estimate " << estimate->parameter() << " (" << estimate->value() << ") was less than the lower bound (" << estimate->lower_bound()
                  << ")";
    } else if (estimate->value() > estimate->upper_bound()) {
      LOG_ERROR() << "The starting value for estimate " << estimate->parameter() << " (" << estimate->value() << ") was greater than the upper bound (" << estimate->upper_bound()
                  << ")";
    }
  }

  LOG_FINE() << "Launching minimiser";
  int               status = 0;
  gammadiff::Engine clGammaDiff;
  clGammaDiff.optimise_finite_differences(call_back, start_values, lower_bounds, upper_bounds, status, max_iterations_, max_evaluations_, gradient_tolerance_, hessian_, 1,
                                          step_size_);


  switch (status) {
    case -1:
      result_ = MinimiserResult::kError;
      break;
    case 0:
      result_ = MinimiserResult::kTooManyIterations;
      break;
    case 1:
      result_ = MinimiserResult::kSuccess;
      break;
    default:
      break;
  }
}

} /* namespace minimisers */
} /* namespace niwa */
#endif
