/*
 * BetaDiff.cpp
 *
 *  Created on: 20/05/2013
 *      Author: Admin
 */
#ifdef USE_BETADIFF
#include "BetaDiff.h"

#include <betadiff.h>

#include "Estimates/Manager.h"
#include "Model/Model.h"
#include "ObjectiveFunction/ObjectiveFunction.h"

namespace isam {
namespace minimisers {

/**
 * Default constructor
 */
BetaDiff::BetaDiff() {
}

/**
 *
 */
void BetaDiff::Validate() {}

class MyModel {
};

class MyObjective {
public:
  Double operator()(const MyModel& model, const dvv& x_unbounded) {
    vector<EstimatePtr> estimates = estimates::Manager::Instance().GetEnabled();

    for (int i = 0; i < x_unbounded.size(); ++i) {
      dvariable estimate = x_unbounded[i + 1];
      estimates[i]->set_value(estimate.x);
    }

    ObjectiveFunction& objective = ObjectiveFunction::Instance();
    Model::Instance()->FullIteration();

    objective.CalculateScore();
    Double score = objective.score();

    return score;
  }
};


/**
 *
 */
void BetaDiff::Execute() {
  estimates::Manager& estimate_manager = estimates::Manager::Instance();

  vector<EstimatePtr> estimates = estimate_manager.GetObjects();


  dvector lower_bounds((int)estimates.size());
  dvector upper_bounds((int)estimates.size());
  dvector start_values((int)estimates.size());

  int i = 0;
  for (EstimatePtr estimate : estimates) {
    ++i;

    if (!estimate->enabled())
      continue;

    lower_bounds[i] = estimate->lower_bound();
    upper_bounds[i] = estimate->upper_bound();
    start_values[i] = AS_DOUBLE(estimate->value());

//    if (estimate->value() < estimate->lower_bound()) {
//      LOG_ERROR("When starting the DESolver minimiser the starting value (" << estimate->value() << ") for estimate "
//          << estimate->parameter() << " was less than the lower bound (" << estimate->lower_bound() << ")");
//    } else if (estimate->value() > estimate->upper_bound()) {
//      LOG_ERROR("When starting the DESolver minimiser the starting value (" << estimate->value() << ") for estimate "
//          << estimate->parameter() << " was greater than the upper bound (" << estimate->upper_bound() << ")");
//    }
  }

  MyModel my_model;
  MyObjective my_objective;

  int convergence = 0;
  int iprint = 0;
  int maxit = 1000;
  int maxfn = 1000;

  double score = optimise<MyModel, MyObjective>(my_model, my_objective, start_values, lower_bounds, upper_bounds, convergence, iprint, maxit, maxfn);
}

} /* namespace reports */
} /* namespace isam */
#endif /* USE_BETADIFF */
