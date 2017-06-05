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
#include "EstimateTransformations/Manager.h"
#include "Model/Model.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace minimisers {

using CppAD::AD;

/**
 * Objective Function
 */
class MyObjective {
public:
  MyObjective(Model* model) : model_(model) { }

  typedef CPPAD_TESTVECTOR( AD<double> ) ADvector;

  void operator()(ADvector& fg, const ADvector& candidates) {
    auto estimates = model_->managers().estimate()->GetIsEstimated();

    for (unsigned i = 0; i < candidates.size(); ++i) {
      Double estimate = candidates[i];
      estimates[i]->set_value(candidates[i]);
    }

    model_->managers().estimate_transformation()->RestoreEstimates();
    model_->FullIteration();

    ObjectiveFunction& objective = model_->objective_function();
    objective.CalculateScore();
    fg[0] = objective.score();

    model_->managers().estimate_transformation()->TransformEstimates();
    return;
  }

private:
  Model* model_;
};

/**
 *
 */
CPPAD::CPPAD(Model* model) : Minimiser(model) {
  parameters_.Bind<string>("solver", &solver_, "The solver to use", "", "mumps");
  parameters_.Bind<string>("retape", &retape_, "Retape? yes or no", "", "true")->set_allowed_values({"true", "false"});
  parameters_.Bind<unsigned>("print_level", &print_level_, "Level of debug to stdout", "", 5u)->set_range(0u, 12u);
  parameters_.Bind<string>("sb", &sb_, "String buffer output?", "", "yes")->set_allowed_values({"yes", "no"});
  parameters_.Bind<unsigned>("max_iter", &max_iter_, "Maximum number of iterations", "", 1000)->set_lower_bound(0u, false);
  parameters_.Bind<Double>("tol", &tol_, "Tolerance for convergence", "", 1e-5);
  parameters_.Bind<Double>("acceptable_obj_change_tol", &acceptable_obj_change_tol_, "", "", 1e+10)->set_lower_bound(0u, false);
  parameters_.Bind<string>("derivative_test", &derivative_test_, "How to test for derivaties", "", "second-order")->set_allowed_values({"second-order", "first-order"});
  parameters_.Bind<Double>("point_pertubation_radius", &point_perturbation_radius_, "", "", 0.0)->set_lower_bound(0.0, true);
}

/**
 *
 */
void CPPAD::Execute() {
  typedef CPPAD_TESTVECTOR( double ) Dvector;

  auto estimate_manager = model_->managers().estimate();
  auto estimates = estimate_manager->GetIsEstimated();

  Dvector lower_bounds(estimates.size());
  Dvector upper_bounds(estimates.size());
  Dvector start_values(estimates.size());

  model_->managers().estimate_transformation()->TransformEstimates();
  for (unsigned i = 0; i < estimates.size(); ++i) {
    lower_bounds[i] = AS_DOUBLE(estimates[i]->lower_bound());
    upper_bounds[i] = AS_DOUBLE(estimates[i]->upper_bound());
    start_values[i] = AS_DOUBLE(estimates[i]->value());
  }

  MyObjective obj(model_);

  // options
  std::string options = "";
  options += "String linear_solver " + solver_ + "\n";
  options += "Retape " + retape_ + "\n"; // retape operation sequence for each new x
  options += "Integer print_level " + utilities::ToInline<unsigned, string>(print_level_) + "\n";
  options += "String sb " + sb_ + "\n";
  options += "Integer max_iter " + utilities::ToInline<unsigned, string>(max_iter_) + "\n";
  options += "Numeric tol " + utilities::ToInline<Double, string>(tol_) + "\n";
  options += "Numeric acceptable_obj_change_tol " + utilities::ToInline<Double, string>(tol_) + "\n";
  options += "String  derivative_test " + derivative_test_ + "\n";
  options += "Numeric point_perturbation_radius " + utilities::ToInline<Double, string>(point_perturbation_radius_) + "\n";

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

  model_->managers().estimate_transformation()->RestoreEstimates();
}

} /* namespace minimisers */
} /* namespace niwa */
#endif /* USE_CPPAD */
#endif /* USE_AUTODIFF */

