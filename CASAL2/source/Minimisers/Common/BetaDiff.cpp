/*
 * BetaDiff.cpp
 *
 *  Created on: 20/05/2013
 *      Author: Admin
 */
#ifdef USE_AUTODIFF
#ifdef USE_BETADIFF
#include "BetaDiff.h"

#include <betadiff.h>

#include "../../Estimates/Manager.h"
#include "../../Model/Model.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"

namespace niwa {
namespace minimisers {

/**
 * Objective Function
 */
class MyModel {};
class MyObjective {
public:
  MyObjective(shared_ptr<Model> model) : model_(model) {}

  Double operator()(const MyModel& model, const dvv& x_unbounded) {
    auto estimates = model_->managers()->estimate()->GetIsEstimated();

    for (int i = 0; i < x_unbounded.size(); ++i) {
      dvariable estimate = x_unbounded[i + 1];
      estimates[i]->set_value(estimate.x);
      LOG_MEDIUM() << estimates[i]->value() << " ";
    }

    model_->FullIteration();
    ObjectiveFunction& objective = model_->objective_function();
    objective.CalculateScore();
    return objective.score();
  }

private:
  shared_ptr<Model> model_;
};

/**
 * Default constructor
 */
BetaDiff::BetaDiff(shared_ptr<Model> model) : Minimiser(model) {
  parameters_.Bind<int>(PARAM_MAX_ITERATIONS, &max_iterations_, "The maximum number of iterations", "", 1000)->set_lower_bound(1);
  parameters_.Bind<int>(PARAM_MAX_EVALUATIONS, &max_evaluations_, "The maximum number of evaluations", "", 4000)->set_lower_bound(1);
  parameters_.Bind<double>(PARAM_TOLERANCE, &gradient_tolerance_, "The tolerance of the gradient for convergence", "", DEFAULT_CONVERGENCE)->set_lower_bound(0.0, false);
}

/**
 * Execute the minimiser
 */
void BetaDiff::Execute() {
  auto estimate_manager = model_->managers()->estimate();
  auto estimates        = estimate_manager->GetIsEstimated();

  dvector lower_bounds((int)estimates.size());
  dvector upper_bounds((int)estimates.size());
  dvector start_values((int)estimates.size());

  LOG_INFO() << "Estimation with the " << PARAM_BETADIFF << " minimiser";

  int i = 0;
  for (auto estimate : estimates) {
    ++i;
    lower_bounds[i] = AS_DOUBLE(estimate->lower_bound());
    upper_bounds[i] = AS_DOUBLE(estimate->upper_bound());
    start_values[i] = AS_DOUBLE(estimate->value());

    if (estimate->value() < estimate->lower_bound()) {
      LOG_ERROR() << "The starting value for estimate " << estimate->parameter() << " (" << estimate->value() << ") was less than the lower bound (" << estimate->lower_bound()
                  << ")";
    } else if (estimate->value() > estimate->upper_bound()) {
      LOG_ERROR() << "The starting value for estimate " << estimate->parameter() << " (" << estimate->value() << ") was greater than the upper bound (" << estimate->upper_bound()
                  << ")";
    }
  }

  MyModel     my_model;
  MyObjective my_objective(model_);

  dmatrix betadiff_hessian(estimates.size(), estimates.size());
  //  dmatrix adolc_hessian(estimates.size(), estimates.size());
  int    convergence = 0;
  double score       = optimise<MyModel, MyObjective>(my_model, my_objective, start_values, lower_bounds, upper_bounds, convergence, 0, max_iterations_, max_evaluations_,
                                                gradient_tolerance_, 0, &betadiff_hessian, 0, 1);
  LOG_FINE() << "complete optimise, get hessian, " << hessian_size_;
  for (int row = 0; row < (int)estimates.size(); ++row) {
    for (int col = 0; col < (int)estimates.size(); ++col) {
      hessian_[row][col] = betadiff_hessian[row + 1][col + 1];
    }
  }
  LOG_FINE() << "return convergence";
  switch (convergence) {
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

}  // namespace minimisers
} /* namespace niwa */
#endif /* USE_BETADIFF */
#endif /* USE_AUTODIFF */
