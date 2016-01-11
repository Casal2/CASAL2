/**
 * @file DESolver.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef USE_AUTODIFF
// Headers
#include "DESolver.h"

#include "Estimates/Manager.h"
#include "Minimisers/Children/DESolver/CallBack.h"
#include "EstimateTransformations/Manager.h"

// Namespaces
namespace niwa {
namespace minimisers {

/**
 * Default constructor
 */
DESolver::DESolver(Model* model) : Minimiser(model) {
  parameters_.Bind<unsigned>(PARAM_POPULATION_SIZE, &population_size_, "The number of candidate solutions to have in the population", "");
  parameters_.Bind<Double>(PARAM_CROSSOVER_PROBABILITY, &crossover_probability_, "TBA", "", 0.9);
  parameters_.Bind<Double>(PARAM_DIFFERENCE_SCALE, &difference_scale_, "The scale to apply to new solutions when comparing candidates", "", 0.02);
  parameters_.Bind<unsigned>(PARAM_MAX_GENERATIONS, &max_generations_, "The maximum number of iterations to run", "");
  parameters_.Bind<Double>(PARAM_TOLERANCE, &tolerance_, "The total variance between the population and best candidate before acceptance", "", 0.01);
  parameters_.Bind<string>(PARAM_METHOD, &method_, "The type of candidate generation method to use", "not_yet_implemented", "");
}

/**
 * Validate the variables giving to this class
 * from the configuration file
 */
void DESolver::DoValidate() {
  if (crossover_probability_ > 1.0 || crossover_probability_ < 0.0) {
    LOG_ERROR_P(PARAM_CROSSOVER_PROBABILITY) << ": crossover_probability must be between 0.0 and 1.0. You entered: " << crossover_probability_;
  }
}

/**
 * Execute our DE Solver minimiser engine
 */
void DESolver::Execute() {
  estimates::Manager& estimate_manager = *model_->managers().estimate();

  vector<double>  lower_bounds;
  vector<double>  upper_bounds;
  vector<double>  start_values;

  model_->managers().estimate_transformation()->TransformEstimates();
  vector<Estimate*> estimates = estimate_manager.GetIsEstimated();
  for (Estimate* estimate : estimates) {
    if (!estimate->estimated())
      continue;

    lower_bounds.push_back(estimate->lower_bound());
    upper_bounds.push_back(estimate->upper_bound());
    start_values.push_back(estimate->value());

    if (estimate->value() < estimate->lower_bound()) {
      LOG_FATAL() << "When starting the DESolver minimiser the starting value (" << estimate->value() << ") for estimate "
          << estimate->parameter() << " was less than the lower bound (" << estimate->lower_bound() << ")";
    } else if (estimate->value() > estimate->upper_bound()) {
      LOG_FATAL() << "When starting the DESolver minimiser the starting value (" << estimate->value() << ") for estimate "
          << estimate->parameter() << " was greater than the upper bound (" << estimate->upper_bound() << ")";
    }
  }

  // Setup Engine
  desolver::CallBack solver = desolver::CallBack(model_, start_values.size(), population_size_, tolerance_);
  solver.Setup(start_values, lower_bounds, upper_bounds, kBest1Exp, difference_scale_, crossover_probability_);

  // Solver
  if (solver.Solve(max_generations_)) {
    LOG_FINE() << "DE Solver has successfully converged";
  } else {
    LOG_FINE() << "DE Solver has failed to converge";
  }

  model_->managers().estimate_transformation()->RestoreEstimates();
}

} /* namespace minimisers */
} /* namespace niwa */
#endif
