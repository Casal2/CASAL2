
/**
 * @file DESolver.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef USE_AUTODIFF
// Headers
#include "DESolver.h"

#include "../../Estimates/Manager.h"
#include "../../Minimisers/Common/DESolver/CallBack.h"

// Namespaces
namespace niwa {
namespace minimisers {

/**
 * Default constructor
 */
DESolver::DESolver(shared_ptr<Model> model) : Minimiser(model) {
  parameters_.Bind<unsigned>(PARAM_POPULATION_SIZE, &population_size_, "The number of candidate solutions to have in the population", "", 25)->set_lower_bound(1);
  parameters_.Bind<double>(PARAM_CROSSOVER_PROBABILITY, &crossover_probability_, "The minimiser's crossover probability", "", 0.9)->set_range(0.0, 1.0);
  parameters_.Bind<double>(PARAM_DIFFERENCE_SCALE, &difference_scale_, "The scale to apply to new solutions when comparing candidates", "", 0.02);
  parameters_.Bind<unsigned>(PARAM_MAX_GENERATIONS, &max_generations_, "The maximum number of iterations to run", "", 1000);
  parameters_.Bind<double>(PARAM_TOLERANCE, &tolerance_, "The total variance between the population and best candidate before acceptance", "", DEFAULT_CONVERGENCE)
      ->set_lower_bound(0.0, false);
  parameters_.Bind<string>(PARAM_METHOD, &method_, "The type of candidate generation method to use", "not_yet_implemented", "");
}

/**
 * Validate the variables giving to this class
 * from the configuration file
 */
void DESolver::DoValidate() {
  if (crossover_probability_ > 1.0 || crossover_probability_ < 0.0) {
    LOG_ERROR_P(PARAM_CROSSOVER_PROBABILITY) << ": crossover_probability (" << crossover_probability_ << ") must be between 0.0 and 1.0 inclusive.";
  }
  if (build_covariance_) {
    build_covariance_ = false;
    LOG_WARNING_P(PARAM_COVARIANCE) << ": cannot be specified as true for DESolver. This minimiser does not support generating a covariance matrix";
  }
}

/**
 * Execute our DE Solver minimiser engine
 */
void DESolver::Execute() {
  estimates::Manager& estimate_manager = *model_->managers()->estimate();

  vector<double> lower_bounds;
  vector<double> upper_bounds;
  vector<double> start_values;

  LOG_INFO() << "Estimation with the " << PARAM_DE_SOLVER << " minimiser";

  vector<Estimate*> estimates = estimate_manager.GetIsEstimated();
  for (Estimate* estimate : estimates) {
    if (!estimate->estimated())
      continue;

    lower_bounds.push_back(estimate->lower_bound());
    upper_bounds.push_back(estimate->upper_bound());
    start_values.push_back(estimate->value());

    if (estimate->value() < estimate->lower_bound()) {
      LOG_ERROR() << "The starting value for estimate " << estimate->parameter() << " (" << estimate->value() << ") was less than the lower bound (" << estimate->lower_bound()
                  << ")";
    } else if (estimate->value() > estimate->upper_bound()) {
      LOG_ERROR() << "The starting value for estimate " << estimate->parameter() << " (" << estimate->value() << ") was greater than the upper bound (" << estimate->upper_bound()
                  << ")";
    }
  }

  // Setup Engine
  desolver::CallBack solver = desolver::CallBack(model_, start_values.size(), population_size_, tolerance_);
  solver.Setup(start_values, lower_bounds, upper_bounds, kBest1Exp, difference_scale_, crossover_probability_);
  LOG_MEDIUM() << "start values now ";
  for (unsigned j = 0; j < start_values.size(); ++j) {
    LOG_MEDIUM() << " start value " << start_values[j];
    estimated_values_.push_back(start_values[j]);
  }
  // Solver
  if (solver.Solve(max_generations_)) {
    result_ = MinimiserResult::kSuccess;
    LOG_IMPORTANT() << "DE Solver has successfully converged";
  } else {
    result_ = MinimiserResult::kError;
    LOG_IMPORTANT() << "DE Solver has failed to converge";
  }
}

} /* namespace minimisers */
} /* namespace niwa */
#endif
