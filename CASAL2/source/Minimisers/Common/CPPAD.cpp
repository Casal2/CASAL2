/**
 * @file CPPAD.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 *
 * CppAD Documentation for Solver options: https://www.coin-or.org/Ipopt/documentation/node40.html
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
 * * For information about what these options do see here https://www.coin-or.org/CppAD/Doc/ipopt_solve.htm
 */
CPPAD::CPPAD(Model* model) : Minimiser(model) {
  parameters_.Bind<string>(PARAM_RETAPE, &retape_, "Retape? yes or no", "", "true")->set_allowed_values({"true", "false"});
  parameters_.Bind<unsigned>(PARAM_PRINT_LEVEL, &print_level_, "Level of debug to stdout", "", 5u)->set_range(0u, 12u);
  parameters_.Bind<string>(PARAM_SB, &sb_, "String buffer output?", "", "yes")->set_allowed_values({"yes", "no"});
  parameters_.Bind<unsigned>(PARAM_MAX_ITER, &max_iter_, "Maximum number of iterations", "", 4000)->set_lower_bound(0u, false);
  parameters_.Bind<Double>(PARAM_TOL, &tol_, "Tolerance for convergence", "", 1e-9);
  parameters_.Bind<Double>(PARAM_ACCEPTABLE_TOL, &acceptable_tol_, "Acceptable tolerance", "", 1e-6)->set_lower_bound(0.0, true);
  parameters_.Bind<Double>(PARAM_ACCEPTABLE_OBJ_CHANGE_TOL, &acceptable_obj_change_tol_, "", "", 1e+20)->set_lower_bound(0u, false);
  parameters_.Bind<string>(PARAM_DERIVATIVE_TEST, &derivative_test_, "How to test for derivaties", "", "none")
      ->set_allowed_values({"none", "first-order", "second-order", "only-second-order"});
  parameters_.Bind<Double>(PARAM_POINT_PERTUBATION_RADIUS, &point_perturbation_radius_, "", "", 0.0)->set_lower_bound(0.0, true);
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
  options += "Retape " + retape_ + "\n"; // retape operation sequence for each new x
  options += "Integer print_level " + utilities::ToInline<unsigned, string>(print_level_) + "\n";
  options += "String sb " + sb_ + "\n";
  options += "Integer max_iter " + utilities::ToInline<unsigned, string>(max_iter_) + "\n";
  options += "Numeric tol " + utilities::ToInline<Double, string>(tol_) + "\n";
  options += "Numeric acceptable_tol " + utilities::ToInline<Double, string>(acceptable_tol_) + "\n";
  options += "Numeric acceptable_obj_change_tol " + utilities::ToInline<Double, string>(acceptable_obj_change_tol_) + "\n";
  options += "String  derivative_test " + derivative_test_ + "\n";
  options += "String check_derivatives_for_naninf yes\n";
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

  LOG_MEDIUM() << "x";
  for(unsigned i = 0; i < solution.x.size(); ++i)
    LOG_MEDIUM() << solution.x[i];
  LOG_MEDIUM() << "g";
  for(unsigned i = 0; i < solution.g.size(); ++i)
    LOG_MEDIUM() << solution.g[i];
  LOG_MEDIUM() << "lambda";
  for(unsigned i = 0; i < solution.lambda.size(); ++i)
    LOG_MEDIUM() << solution.lambda[i];
  LOG_MEDIUM() << "zl";
  for(unsigned i = 0; i < solution.zl.size(); ++i)
    LOG_MEDIUM() << solution.zl[i];
  LOG_MEDIUM() << "zu";
  for(unsigned i = 0; i < solution.zu.size(); ++i)
    LOG_MEDIUM() << solution.zu[i];
  LOG_MEDIUM() << "obj";
  LOG_MEDIUM() << solution.obj_value;
  LOG_MEDIUM() << "status";
  /*
  switch(solution.status)
  { // convert status from Ipopt enum to solve_result<Dvector> enum
    case status_type::success:
    cerr << "success\n";
    break;
    case maxiter_exceeded:
    cerr << "max iter exceeded\n";
    break;

    case CppAD::ipopt::solve_result::status_type::stop_at_tiny_step:
    cerr << "stop at tiny step\n";
    break;

    case CppAD::ipopt::solve_result::status_type::stop_at_acceptable_point:
    cerr << "stop at acceptable point\n";
    break;

    case CppAD::ipopt::solve_result::status_type::local_infeasibility:
    cerr << "local infeasibility\n";
    break;

    case CppAD::ipopt::solve_result::status_type::user_requested_stop:
    cerr << "user requested stop\n";
    break;

    case CppAD::ipopt::solve_result::status_type::diverging_iterates:
    cerr << "diverging iterates\n";
    break;

    case CppAD::ipopt::solve_result::status_type::restoration_failure:
    cerr << "restoration failure\n";
    break;

    case CppAD::ipopt::solve_result::status_type::error_in_step_computation:
    cerr << "error in step computation\n";
    break;

    case CppAD::ipopt::solve_result::status_type::invalid_number_detected:
    cerr << "invalid number detected\n";
    break;

    case CppAD::ipopt::solve_result::status_type::internal_error:
    cerr << "internal error\n";
    break;

    default:
    cerr << "unknown error\n";
  }
*/
  model_->managers().estimate_transformation()->RestoreEstimates();
}

} /* namespace minimisers */
} /* namespace niwa */
#endif /* USE_CPPAD */
#endif /* USE_AUTODIFF */

