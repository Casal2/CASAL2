/*
 * ADMB.cpp
 *
 *  Created on: 7/05/2013
 *      Author: Admin
 */
#ifdef USE_ADMB
#include "ADMB.h"

#include "Estimates/Manager.h"
#include "Model/Model.h"
#include "ObjectiveFunction/ObjectiveFunction.h"

// namespaces
namespace isam {
namespace minimisers {


/**
 * Default constructor
 */
ADMB::ADMB() {
}

/**
 * Validate configuration
 */
void ADMB::Validate() {
  Minimiser::Validate();
}

/**
 * User function
 */
double UserFunction(const dvar_vector& parameters) {
  vector<EstimatePtr> estimates = estimates::Manager::Instance().GetEnabled();

  if (parameters.size() != (int)estimates.size()) {
    LOG_CODE_ERROR("The number of enabled estimates does not match the number of test solution values");
  }

  cout << "Sizes: " << estimates.size() << " : " << parameters.size() << endl;

  dvariable penalty = 0;
  for (int i = 0; i < (int)estimates.size(); ++i) {
    double lower_bound = estimates[i]->lower_bound();
    double upper_bound = estimates[i]->upper_bound();

    cout << "L/U: " << lower_bound << " / " << upper_bound << endl;
    dvariable constrained_value = boundp(parameters.elem(i+1), lower_bound, upper_bound, penalty);
    estimates[i]->set_value(constrained_value);
    cout << "Constrained: " << constrained_value << " vs " << parameters.elem(i+1) << endl;
    cout << "Penalty: " << penalty << endl;
    cout << endl;
  }

  ObjectiveFunction& objective = ObjectiveFunction::Instance();
  Model::Instance()->FullIteration();

  objective.CalculateScore();
  dvariable score = objective.score() + penalty;

  return value(score);
}

/**
 * Execute the minimisers
 */
void ADMB::Execute() {
  estimates::Manager& estimate_manager = estimates::Manager::Instance();

  vector<Double>  lower_bounds;
  vector<Double>  upper_bounds;
  vector<Double>  start_values;

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

  int nvar = (int)start_values.size();
  independent_variables x(1, nvar);

//  for (int i = 1; i <= x.size(); ++i)
//    x. = start_values[i - 1]; //boundpin(start_values[i-1], value(lower_bounds[i-1]), value(upper_bounds[i-1]));

  fmm fmc(nvar); // creates the function minimizing control structure
  double minimum_value = fmc.minimizeNoGS(x, UserFunction);

  cout << "The minimum value = " << minimum_value << "at x =\n" << x << "\n";
  cout << "Finished minimisation: " << fmc.ireturn << endl;
}

} /* namespace minimisers */
} /* namespace isam */
#endif /* USE_ADMB */
