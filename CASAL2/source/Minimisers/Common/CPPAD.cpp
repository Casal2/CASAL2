/**
 * @file CPPAD.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science (c)2014 - www.niwa.co.nz
 *
 *
 * CppAD Documentation for Solver options: https://www.coin-or.org/Ipopt/documentation/node40.html
 */
#ifdef USE_AUTODIFF
#ifdef USE_CPPAD

// headers
#include "CPPAD.h"

#include <cppad/ipopt/solve.hpp>
#include <limits>
#include <numeric>

#include "../../EstimateTransformations/Manager.h"
#include "../../Estimates/Manager.h"
#include "../../Model/Model.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../Utilities/To.h"

// namespaces
namespace niwa {
namespace minimisers {

using CppAD::AD;

/**
 * Objective Function class
 */
class MyObjective {
public:
  MyObjective(shared_ptr<Model> model) : model_(model) {}

  typedef CPPAD_TESTVECTOR(AD<double>) ADvector;

  void operator()(ADvector& fg, const ADvector& candidates) {
    auto estimates = model_->managers()->estimate()->GetIsEstimated();

    for (unsigned i = 0; i < candidates.size(); ++i) {
      Double estimate = candidates[i];
      estimates[i]->set_value(candidates[i]);
    }

    model_->managers()->estimate_transformation()->RestoreEstimates();
    model_->FullIteration();

    ObjectiveFunction& objective = model_->objective_function();
    objective.CalculateScore();
    fg[0] = objective.score();

    model_->managers()->estimate_transformation()->TransformEstimates();
    return;
  }

private:
  shared_ptr<Model> model_;
};

/**
 * Default constructor
 *
 * For information about these options see https://www.coin-or.org/CppAD/Doc/ipopt_solve.htm
 */
CPPAD::CPPAD(shared_ptr<Model> model) : Minimiser(model) {
  parameters_.Bind<string>(PARAM_RETAPE, &retape_, "Retape?", "", "true")->set_allowed_values({"true", "false"});
  parameters_.Bind<unsigned>(PARAM_PRINT_LEVEL, &print_level_, "The level of debug to stdout", "", 5u)->set_range(0u, 12u);
  parameters_.Bind<string>(PARAM_SB, &sb_, "String buffer output?", "", "yes")->set_allowed_values({"yes", "no"});
  parameters_.Bind<string>(PARAM_PIDI, &pidi_, "Print iteration diagnostic information?", "", "yes")->set_allowed_values({"yes", "no"});
  parameters_.Bind<unsigned>(PARAM_MAX_ITERATIONS, &max_iter_, "The maximum number of iterations", "", 100)->set_lower_bound(1u);
  parameters_.Bind<double>(PARAM_TOLERANCE, &tol_, "The tolerance for convergence", "", 1e-9)->set_lower_bound(0.0, false);
  parameters_.Bind<double>(PARAM_ACCEPTABLE_TOL, &acceptable_tol_, "The acceptable tolerance", "", 1e-6)->set_lower_bound(0.0, false);
  parameters_.Bind<double>(PARAM_ACCEPTABLE_OBJ_CHANGE_TOL, &acceptable_obj_change_tol_, "", "", 1e+20)->set_lower_bound(0.0, false);
  parameters_.Bind<string>(PARAM_DERIVATIVE_TEST, &derivative_test_, "How to test for derivatives", "", "first-order")
      ->set_allowed_values({"none", "first-order", "second-order", "only-second-order"});
  parameters_.Bind<Double>(PARAM_POINT_PERTUBATION_RADIUS, &point_perturbation_radius_, "The point perturbation radius", "", 0.0)->set_lower_bound(0.0, true);
}

/**
 * Execute
 */
void CPPAD::Execute() {
  typedef CPPAD_TESTVECTOR(double) Dvector;

  auto estimate_manager = model_->managers()->estimate();
  auto estimates        = estimate_manager->GetIsEstimated();
  auto num_estimates    = estimates.size();

  Dvector lower_bounds(num_estimates);
  Dvector upper_bounds(num_estimates);
  Dvector start_values(num_estimates);

  model_->managers()->estimate_transformation()->TransformEstimates();
  for (unsigned i = 0; i < num_estimates; ++i) {
    lower_bounds[i] = AS_DOUBLE(estimates[i]->lower_bound());
    upper_bounds[i] = AS_DOUBLE(estimates[i]->upper_bound());
    start_values[i] = AS_DOUBLE(estimates[i]->value());
  }

  MyObjective obj(model_);

  // options
  // see https://coin-or.github.io/Ipopt/OPTIONS.html and https://coin-or.github.io/Ipopt/OUTPUT.html
  std::string options = "";
  options += "Retape " + retape_ + "\n";  // retape operation sequence for each new x
  options += "Integer print_level " + utilities::ToInline<unsigned, string>(print_level_) + "\n";
  options += "String sb " + sb_ + "\n";
  options += "String print_info_string " + pidi_ + "\n";
  options += "Integer max_iter " + utilities::ToInline<unsigned, string>(max_iter_) + "\n";
  options += "Numeric tol " + utilities::ToInline<double, string>(tol_) + "\n";
  options += "Numeric acceptable_tol " + utilities::ToInline<double, string>(acceptable_tol_) + "\n";
  options += "Numeric acceptable_obj_change_tol " + utilities::ToInline<double, string>(acceptable_obj_change_tol_) + "\n";
  options += "String derivative_test " + derivative_test_ + "\n";
  options += "String check_derivatives_for_naninf yes\n";
  options += "Numeric point_perturbation_radius " + utilities::ToInline<Double, string>(point_perturbation_radius_) + "\n";

  CppAD::ipopt::solve_result<Dvector> solution;

  Dvector g(1);
  g[0] = 0;

  Dvector gl(1);
  gl[0] = std::numeric_limits<double>::min();
  Dvector gu(1);
  gu[0] = std::numeric_limits<double>::max();

  CppAD::ipopt::solve<Dvector, MyObjective>(options, start_values, lower_bounds, upper_bounds, gl, gu, obj, solution);

  Dvector x(solution.x.size());

  LOG_MEDIUM() << "x: the approximation solution";
  for (unsigned i = 0; i < solution.x.size(); ++i) {
    x[i] = solution.x[i];
    LOG_MEDIUM() << solution.x[i];
  }
  LOG_MEDIUM() << "zl: Lagrange multipliers corresponding to lower bounds on x";
  for (unsigned i = 0; i < solution.zl.size(); ++i) LOG_MEDIUM() << solution.zl[i];
  LOG_MEDIUM() << "zu: Lagrange multipliers corresponding to upper bounds on x";
  for (unsigned i = 0; i < solution.zu.size(); ++i) LOG_MEDIUM() << solution.zu[i];

  LOG_MEDIUM() << "g: value of g(x)";
  for (unsigned i = 0; i < solution.g.size(); ++i) LOG_MEDIUM() << solution.g[i];
  LOG_MEDIUM() << "lambda: Lagrange multipliers corresponding to constraints on g(x)";
  for (unsigned i = 0; i < solution.lambda.size(); ++i) LOG_MEDIUM() << solution.lambda[i];

  LOG_MEDIUM() << "objective function value: value of f(x)";
  LOG_MEDIUM() << solution.obj_value;
  LOG_MEDIUM() << "status";
  /*
   *
   */
  switch (solution.status) {  // convert status from Ipopt enum to solve_result<Dvector> enum
    case solution.status_type::success:
      result_ = MinimiserResult::kSuccess;
      LOG_MEDIUM() << "success";
      break;

    case solution.status_type::maxiter_exceeded:
      result_ = MinimiserResult::kTooManyIterations;
      LOG_MEDIUM() << "max iter exceeded";
      break;

    case solution.status_type::stop_at_tiny_step:
      result_ = MinimiserResult::kStepSizeTooSmallSuccess;
      LOG_MEDIUM() << "stop at tiny step";
      break;

    case solution.status_type::stop_at_acceptable_point:
      result_ = MinimiserResult::kSuccess;
      LOG_MEDIUM() << "stop at acceptable point";
      break;

    case solution.status_type::local_infeasibility:
      result_ = MinimiserResult::kError;
      LOG_MEDIUM() << "local infeasibility";
      break;

    case solution.status_type::user_requested_stop:
      result_ = MinimiserResult::kError;
      LOG_MEDIUM() << "user requested stop";
      break;

    case solution.status_type::feasible_point_found:
      result_ = MinimiserResult::kSuccess;
      LOG_MEDIUM() << "feasible point found";
      break;

    case solution.status_type::diverging_iterates:
      result_ = MinimiserResult::kError;
      LOG_MEDIUM() << "diverging iterates";
      break;

    case solution.status_type::restoration_failure:
      result_ = MinimiserResult::kError;
      LOG_MEDIUM() << "restoration failure";
      break;

    case solution.status_type::error_in_step_computation:
      result_ = MinimiserResult::kError;
      LOG_MEDIUM() << "error in step computation";
      break;

    case solution.status_type::invalid_number_detected:
      result_ = MinimiserResult::kError;
      LOG_MEDIUM() << "invalid number detected";
      break;

    case solution.status_type::too_few_degrees_of_freedom:
      result_ = MinimiserResult::kError;
      LOG_MEDIUM() << "too few degrees of freedom";
      break;

    case solution.status_type::internal_error:
      result_ = MinimiserResult::kError;
      LOG_MEDIUM() << "internal error";
      break;

    default:
      result_ = MinimiserResult::kError;
      LOG_MEDIUM() << "unknown error, status type: " << solution.status;
  }

  // see https://coin-or.github.io/CppAD/doc/hessian.htm
  // and https://sourceforge.net/p/casadi/discussion/1271244/thread/c6df4d27/
  // unsigned num_params = x.size();
  // vector<double> w(num_params, 1.0);
  // auto hessian = obj.Hessian(x, w); // vector of length n x n
  // if (hessian.size() > 0) {
  //   for (unsigned i = 0; i < num_params; ++i)
  //     for (unsigned j = 0; j < num_params; ++j)
  //       hessian_[i][j] = hessian[num_params * j + i];
  // }

  model_->managers()->estimate_transformation()->RestoreEstimates();
}

} /* namespace minimisers */
} /* namespace niwa */
#endif /* USE_CPPAD */
#endif /* USE_AUTODIFF */
