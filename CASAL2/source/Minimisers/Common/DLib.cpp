
/**
 * @file DLib.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 14/03/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifndef USE_AUTODIFF

// headers
#include "DLib.h"

#include <functional>

#include "Estimates/Manager.h"
#include "Minimisers/Common/DLib/CallBack.h"
#include "Utilities/Math.h"
#include "EstimateTransformations/Manager.h"
#include "Model/Model.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Utilities/Math.h"

// namespaces
namespace niwa {
namespace minimisers {

using namespace dlib;


const column_vector DLib::DLibCalculateGradient(const column_vector& estimate_original_values) {
  ::dlib::matrix<double, 0, 1> gradient_values(estimate_original_values.size());

  vector<Estimate*> estimates = model_->managers().estimate()->GetIsEstimated();

  vector<Double> estimate_values;
  for (unsigned i = 0; i < estimates.size(); ++i) {
    estimate_values.push_back(utilities::math::scale_value(estimates[i]->value(), estimates[i]->lower_bound(), estimates[i]->upper_bound()));
  }

  Double eps = 1e-2;
  Double plus_eps = 0.0;
  Double minus_eps = 0;
  Double original_estimate_value = 0.0;
  Double penalty = 0.0;

  for (unsigned i = 0; i < estimates.size(); ++i) {
    // Backup Orig Value
    original_estimate_value       = estimate_values[i];

    /**
     * Run Model with plus eps
     */
    penalty = 0.0;
    estimate_values[i] = original_estimate_value + eps;
    for (unsigned j = 0; j < estimates.size(); ++j) {
      Double p = 0;
      Double value = utilities::math::unscale_value(estimate_values[j], p, estimates[j]->lower_bound(), estimates[j]->upper_bound());
      estimates[j]->set_value(value);
      if (i == j) penalty += p;
    }

    model_->managers().estimate_transformation()->RestoreEstimates();
    model_->FullIteration();

    ObjectiveFunction& objective = model_->objective_function();
    objective.CalculateScore();

    model_->managers().estimate_transformation()->TransformEstimates();
    plus_eps = objective.score() + penalty;

    /**
     * Run The Model with minus eps
     */
    penalty = 0.0;
    estimate_values[i] = original_estimate_value - eps;
    for (unsigned j = 0; j < estimates.size(); ++j) {
      Double p = 0;
      Double value = utilities::math::unscale_value(estimate_values[j], p, estimates[j]->lower_bound(), estimates[j]->upper_bound());
      estimates[j]->set_value(value);
      if (i == j) penalty += p;
    }

    model_->managers().estimate_transformation()->RestoreEstimates();
    model_->FullIteration();

    objective.CalculateScore();

    model_->managers().estimate_transformation()->TransformEstimates();
    minus_eps = objective.score() + penalty;

    /**
     * Populate Gradient Object
     * (f(x+eps)-f(x-eps))/(2*eps)
     */
    gradient_values(i)  = (plus_eps - minus_eps) / (2*eps);
    estimate_values[i]  = original_estimate_value;
  }

  return gradient_values;
}

/**
 * default constructor
 */
DLib::DLib(Model* model) : Minimiser(model) {

  parameters_.Bind<string>(PARAM_MINIMISATION_TYPE, &minimisation_type_, "The type of minimisation to use", "", PARAM_MIN_USING_APPROX_DERIVATIVES)
      ->set_allowed_values({ PARAM_MIN_USING_APPROX_DERIVATIVES, PARAM_MINIMISATION, PARAM_MIN_BOX_CONSTRAINED, PARAM_MIN_TRUST_REGION, PARAM_MIN_BOBYQA, PARAM_MIN_GLOBAL });
  parameters_.Bind<string>(PARAM_SEARCH_STRATEGY, &search_strategy_, "The type of search strategy to use", "", PARAM_SEARCH_BFGS)
      ->set_allowed_values({ PARAM_SEARCH_BFGS, PARAM_SEARCH_LBFGS, PARAM_SEARCH_CG, PARAM_SEARCH_NEWTON });
  parameters_.Bind<double>(PARAM_TOLERANCE, &gradient_tolerance_, "Tolerance of the gradient for convergence", "", 1e-7);
  parameters_.Bind<unsigned>(PARAM_LBFGS_MAX_SIZE, &lbfgs_max_size_, "Max Size for LBFGS search strategy", "", 1);
  parameters_.Bind<unsigned>(PARAM_BOBYQA_INTERPOLATION_POINTS, &bobyqa_interpolation_points_, "BOBYQA interpolation points", "", 5u);
  parameters_.Bind<double>(PARAM_BOBYQA_INITIAL_TRUST_RADIUS, &bobyqa_initial_trust_radius_, "BOBYQA initial trust radius", "", 1e-2);
  parameters_.Bind<double>(PARAM_BOBYQA_STOPPING_TRUST_RADIUS, &bobyqa_stopping_trust_radius_, "BOBYQA stopping trust radius", "", 1e-6);
  parameters_.Bind<double>(PARAM_BOBYQA_MAX_EVALUATIONS, &bobyqa_max_evaluations_, "BOBYQA max objective evaluations", "", 4000);
  parameters_.Bind<bool>(PARAM_VERBOSE, &verbose_, "Print debug of objective function calls", "", false);
}

/**
 *
 */
void DLib::Execute() {
  LOG_FINE() << "Executing DLib Minimiser";
  using namespace std::placeholders;

  // Variables
  dlib::Callback  call_back(model_);

  estimates::Manager& estimate_manager = *model_->managers().estimate();
  vector<Estimate*> estimates = estimate_manager.GetIsEstimated();

  ::dlib::matrix<double, 0, 1> start_values(estimates.size());
  ::dlib::matrix<double, 0, 1> lower_bounds(estimates.size());
  ::dlib::matrix<double, 0, 1> upper_bounds(estimates.size());

  model_->managers().estimate_transformation()->TransformEstimates();
  unsigned i = 0;
  for (Estimate* estimate : estimates) {
    if (!estimate->estimated())
      continue;

    lower_bounds(i) = estimate->lower_bound();
    upper_bounds(i) = estimate->upper_bound();
    start_values(i) = estimate->value();

    if (estimate->value() < estimate->lower_bound()) {
      LOG_ERROR() << "When starting the DLib minimiser the starting value (" << estimate->value() << ") for estimate "
          << estimate->parameter() << " was less than the lower bound (" << estimate->lower_bound() << ")";
    } else if (estimate->value() > estimate->upper_bound()) {
      LOG_ERROR() << "When starting the DLib minimiser the starting value (" << estimate->value() << ") for estimate "
          << estimate->parameter() << " was greater than the upper bound (" << estimate->upper_bound() << ")";
    }

    ++i;
  }

  decltype(::dlib::objective_delta_stop_strategy(gradient_tolerance_)) gradient_stop_tolerance;
  if (verbose_)
  gradient_stop_tolerance = gradient_stop_tolerance.be_verbose();

  LOG_MEDIUM() << minimisation_type_ << " : " << search_strategy_;
  try {
    if (minimisation_type_ == PARAM_MIN_USING_APPROX_DERIVATIVES) {
      if (search_strategy_ == PARAM_SEARCH_BFGS) {
        ::dlib::find_min_using_approximate_derivatives(::dlib::bfgs_search_strategy(),
            gradient_stop_tolerance,
            dlib::Callback(model_), start_values, -1);
      } else if (search_strategy_ == PARAM_SEARCH_CG) {
        ::dlib::find_min_using_approximate_derivatives(::dlib::cg_search_strategy(),
            gradient_stop_tolerance,
            dlib::Callback(model_), start_values, -1);
      } else if (search_strategy_ == PARAM_SEARCH_LBFGS) {
        ::dlib::find_min_using_approximate_derivatives(::dlib::lbfgs_search_strategy(lbfgs_max_size_),
            gradient_stop_tolerance,
            dlib::Callback(model_), start_values, -1);
      } else if (search_strategy_ == PARAM_SEARCH_NEWTON)
        LOG_FATAL_P(PARAM_SEARCH_STRATEGY) << "Newton is not supported with " << PARAM_MIN_USING_APPROX_DERIVATIVES;

    } else if (minimisation_type_ == PARAM_MINIMISATION) {
      if (search_strategy_ == PARAM_SEARCH_BFGS) {
        ::dlib::find_min(::dlib::bfgs_search_strategy(),
            gradient_stop_tolerance,
            dlib::Callback(model_),
            std::bind(&DLib::DLibCalculateGradient, this, _1),
            start_values, -1.0);
      } else if (search_strategy_ == PARAM_SEARCH_CG) {
        ::dlib::find_min(::dlib::cg_search_strategy(),
            gradient_stop_tolerance,
            dlib::Callback(model_),
            std::bind(&DLib::DLibCalculateGradient, this, _1),
            start_values, -1);
      } else if (search_strategy_ == PARAM_SEARCH_LBFGS) {
        ::dlib::find_min(::dlib::lbfgs_search_strategy(lbfgs_max_size_),
            gradient_stop_tolerance,
            dlib::Callback(model_),
            std::bind(&DLib::DLibCalculateGradient, this, _1),
            start_values, -1);
      } else if (search_strategy_ == PARAM_SEARCH_NEWTON)
        LOG_FATAL_P(PARAM_SEARCH_STRATEGY) << "Newton is not supported with " << PARAM_MIN_USING_APPROX_DERIVATIVES;

    } else if (minimisation_type_ == PARAM_MIN_BOBYQA) {
        ::dlib::find_min_bobyqa(dlib::Callback(model_),
                          start_values,
                          //(start_values.size()+1)*(start_values.size()+2)/2,
                          bobyqa_interpolation_points_, // number of interpolation points
                          lower_bounds,  // lower bound constraint
                          upper_bounds,   // upper bound constraint
                          bobyqa_initial_trust_radius_,    // initial trust region radius
                          bobyqa_stopping_trust_radius_,  // stopping trust region radius
                          bobyqa_max_evaluations_    // max number of objective function evaluations
                          );
    } else
      LOG_FATAL_P(PARAM_MINIMISATION_TYPE) << "has not been implemented yet";

  } catch(::dlib::error &ex) {
    LOG_FATAL() << "DLib has experienced an error. Please note that this is NOT a Casal2 error.\nConsult DLib documentation for a solution:\n"
    << "DLib Error: " << ex.what();
  }

  model_->managers().estimate_transformation()->RestoreEstimates();

  result_ = MinimiserResult::kSuccess;
}


} /* namespace minimisers */
} /* namespace niwa */

#endif /* NOT USE_AUTODIFF */
