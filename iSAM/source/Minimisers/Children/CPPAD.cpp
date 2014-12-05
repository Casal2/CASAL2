/**
 * @file CPPAD.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef USE_AUTODIFF
#ifdef USE_CPPAD

// headers
#include "CPPAD.h"

#include <numeric>
#include <limits>
#include <cppad/ipopt/solve.hpp>

#include "Estimates/Manager.h"
#include "Model/Model.h"
#include "ObjectiveFunction/ObjectiveFunction.h"

// namespaces
namespace niwa {
namespace minimisers {

using CppAD::AD;

/**
 * Objective Function
 */
class MyObjective {
public:
  typedef CPPAD_TESTVECTOR( AD<double> ) ADvector;

  void operator()(ADvector& fg, const ADvector& candidates) {
    vector<EstimatePtr> estimates = estimates::Manager::Instance().GetEnabled();

    for (unsigned i = 0; i < candidates.size(); ++i) {
      Double estimate = candidates[i];
      estimates[i]->SetTransformedValue(candidates[i]);
    }

    ObjectiveFunction& objective = ObjectiveFunction::Instance();
    Model::Instance()->FullIteration();

    objective.CalculateScore();
    fg[0] = objective.score();

    return;
  }
};

/**
 *
 */
CPPAD::CPPAD() {
}

/**
 *
 */
void CPPAD::Execute() {
  typedef CPPAD_TESTVECTOR( double ) Dvector;

  estimates::Manager& estimate_manager = estimates::Manager::Instance();

  vector<EstimatePtr> estimates = estimate_manager.GetObjects();

  Dvector lower_bounds(estimates.size());
  Dvector upper_bounds(estimates.size());
  Dvector start_values(estimates.size());

  for (unsigned i = 0; i < estimates.size(); ++i) {
    lower_bounds[i] = AS_DOUBLE(estimates[i]->lower_bound());
    upper_bounds[i] = AS_DOUBLE(estimates[i]->upper_bound());
    start_values[i] = AS_DOUBLE(estimates[i]->GetTransformedValue());
  }

  MyObjective obj;

  // options
  std::string options;
  // retape operation sequence for each new x
  options += "Retape  true\n";
  // turn off any printing
//  options += "Integer print_level   0\n";
//  options += "String  sb          yes\n";
  // maximum number of iterations
  options += "Integer max_iter      1000\n";
  // approximate accuracy in first order necessary conditions;
  // see Mathematical Programming, Volume 106, Number 1,
  // Pages 25-57, Equation (6)
  options += "Numeric tol           1e-9\n";
  // derivative testing
  options += "String  derivative_test            second-order\n";
  // maximum amount of random pertubation; e.g.,
  // when evaluation finite diff
  options += "Numeric point_perturbation_radius  0.\n";

  CppAD::ipopt::solve_result<Dvector> solution;

  Dvector g(1);
  g[0] = 0;

  Dvector gl(1);
  gl[0] = std::numeric_limits<double>::min();
  Dvector gu(1);
  gu[0] = std::numeric_limits<double>::max();

  CppAD::ipopt::solve<Dvector, MyObjective>(
      options, start_values, lower_bounds, upper_bounds, gl, gu, obj, solution
    );
}

} /* namespace minimisers */
} /* namespace niwa */
#endif /* USE_CPPAD */
#endif /* USE_AUTODIFF */
