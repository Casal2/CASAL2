/**
 * @file HamiltonianMonteCarlo.cpp
 * @author Scott Rasmussen (scott@zaita.com)
 * @brief
 * @version 0.1
 * @date 2021-04-24
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef USE_AUTODIFF

// headers
#include "HamiltonianMonteCarlo.h"

#include <boost/numeric/ublas/blas.hpp>

#include "../../EstimateTransformations/Manager.h"
#include "../../Estimates/Manager.h"
#include "../../Minimisers/Manager.h"
#include "../../Model/Model.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../Reports/Manager.h"
#include "../../Utilities/Gradient.h"
#include "../../Utilities/Math.h"
#include "../../Utilities/RandomNumberGenerator.h"
#include "../../Utilities/Vector.h"

// namespaces
namespace niwa::mcmcs {

namespace math  = niwa::utilities::math;
namespace ublas = boost::numeric::ublas;
using niwa::utilities::Vector_Add;
using niwa::utilities::Vector_Debug;
using niwa::utilities::Vector_Scale;

/**
 * @brief norm2. Return cummulative squard values of
 * the vector
 *
 * @param target
 * @return double
 */
double norm2(const std::vector<double>& target) {
  double result = 0.0;
  for (auto d : target) result += d * d;
  return result;
}

/**
 * @brief Construct a new Hamiltonian Monte Carlo:: Hamiltonian Monte Carlo object
 *
 * @param model
 */
HamiltonianMonteCarlo::HamiltonianMonteCarlo(shared_ptr<Model> model) : MCMC(model) {
  parameters_.Bind<unsigned>(PARAM_LEAPFROG_STEPS, &leapfrog_steps_, "Number of leapfrog steps", "", 1u)->set_lower_bound(0, false);
  parameters_.Bind<double>(PARAM_LEAPFROG_DELTA, &leapfrog_delta_, "Amount to leapfrog per step", "", 1e-7)->set_lower_bound(0, false);
  parameters_.Bind<double>(PARAM_GRADIENT_STEP_SIZE, &gradient_step_size_, "Step size to use when calculating gradient", "", 1e-7)->set_lower_bound(1e-13, true);
}

/**
 * @brief
 *
 */
constexpr double PI = 3.1415926535897932384626433832795028;

double scale(double value, double min, double max) {
  double scaled = tan(((value - min) / (max - min) - 0.5) * PI);
  return scaled;
}

double unscale(double value, double min, double max) {
  double unscaled = ((atan(value) / PI) + 0.5) * (max - min) + min;
  return unscaled;
}

/**
 * @brief Generate new candidates using the parent method then scale them so that
 * HMC can work entirely within scaled space when doing the leap frogs
 */
void HamiltonianMonteCarlo::GeneratedNewScaledCandidates() {
  GenerateNewCandidates();
  for (unsigned i = 0; i < candidates_.size(); ++i) {
    candidates_[i] = scale(candidates_[i], estimate_lower_bounds_[i], estimate_upper_bounds_[i]);
  }
}

/**
 * @brief Execute the HMC algorithm using a threaded system
 *
 * @param thread_pool Pool of threads to use when running model iterations
 */
void HamiltonianMonteCarlo::DoExecute(shared_ptr<ThreadPool> thread_pool) {
  LOG_MEDIUM() << "Starting Hamiltonian Monte Carlo";
  ObjectiveFunction&                obj_function   = model_->objective_function();
  utilities::RandomNumberGenerator& rng            = utilities::RandomNumberGenerator::Instance();
  double                            previous_score = obj_function.score();
  double                            delta_half     = leapfrog_delta_ / 2;
  vector<double>                    gradient(candidates_.size(), 0);
  vector<double>                    scores(100, 0u);
  vector<vector<double>>            candidates;

  estimate_lower_bounds_ = model_->managers()->estimate()->lower_bounds();
  estimate_upper_bounds_ = model_->managers()->estimate()->upper_bounds();

  // Lambda to increment vector e by scaled g
  auto increment_vector = [](vector<double> e, vector<double> g, double delta) {
    vector<double> result;
    for (unsigned i = 0; i < e.size(); ++i) {
      result.push_back(e[i] + (g[i] * delta));
    }

    return result;
  };

  // Lambda to easily run a model run with some iterations
  auto quick_run = [this](const vector<double>& candidates) {
    for (unsigned i = 0; i < candidates.size(); ++i) estimates_[i]->set_value(candidates[i]);

    model_->FullIteration();
    model_->objective_function().CalculateScore();
    double score = model_->objective_function().score();
    return score;
  };

  do {
    /**
     * @brief Hamiltonian Monte Carlo is a leap-frog style MCMC where
     * it will do multiple steps in a direction checking the gradient
     * along the way so that it is always attempting to turn towards
     * the minimum
     *
     * Reference implementation for each leapfrog
     * p.increment(self.gradLogDensity(q).scale(self.dt / 2));
     * q.increment(p.scale(self.dt));
     * p.increment(self.gradLogDensity(q).scale(self.dt / 2));
     *
     */
    vector<double> q0 = chain_.rbegin()->values_;
    GeneratedNewScaledCandidates();
    vector<double> q = q0;
    vector<double> p = candidates_;

    for (unsigned i = 0; i < leapfrog_steps_; ++i) {
      //**********************************************************
      // p.increment(self.gradLogDensity(q).scale(self.dt / 2));
      double score = quick_run(q);
      gradient     = utilities::gradient::Calculate(thread_pool, q, estimate_lower_bounds_, estimate_upper_bounds_, gradient_step_size_, score, false);
      p            = increment_vector(p, gradient, delta_half);

      //**********************************************************
      // q.increment(p.scale(self.dt));
      math::scale_vector(q, estimate_lower_bounds_, estimate_upper_bounds_);
      q = increment_vector(q, p, leapfrog_delta_);
      math::unscale_vector(q, estimate_lower_bounds_, estimate_upper_bounds_);
      score = quick_run(q);

      //**********************************************************
      // p.increment(self.gradLogDensity(q).scale(self.dt / 2));
      gradient = utilities::gradient::Calculate(thread_pool, q, estimate_lower_bounds_, estimate_upper_bounds_, gradient_step_size_, score, false);
      math::scale_vector(q, estimate_lower_bounds_, estimate_upper_bounds_);
      p = increment_vector(q, gradient, delta_half);
      math::unscale_vector(q, estimate_lower_bounds_, estimate_upper_bounds_);
    }  // for (unsigned i = 0; i < leapfrog_steps_; ++i)

    // Determine if we're going to accept this jump or not
    double ratio       = 1.0;
    double q_score     = quick_run(q);
    double rng_uniform = rng.uniform();
    if (q_score >= previous_score) {
      ratio = exp(previous_score - q_score);
    }

    // Check if we accept this jump
    if (math::IsEqual(ratio, 1.0) || rng_uniform < ratio) {
      LOG_MEDIUM() << "Accepting Jump (ratio: " << ratio << ", rng: " << rng_uniform << ")" << endl;
      mcmc::ChainLink new_link{.iteration_                   = jumps_,
                               .score_                       = obj_function.score(),
                               .likelihood_                  = obj_function.likelihoods(),
                               .prior_                       = obj_function.priors(),
                               .penalty_                     = obj_function.penalties(),
                               .additional_priors_           = obj_function.additional_priors(),
                               .jacobians_                   = obj_function.jacobians(),
                               .acceptance_rate_             = 0,
                               .acceptance_rate_since_adapt_ = 0,
                               .step_size_                   = step_size_,
                               .values_                      = q};
      chain_.push_back(new_link);
    } else {
      LOG_MEDIUM() << "Rejecting Jump (ratio: " << ratio << ", rng: " << rng_uniform << ")" << endl;
      // Copy the last chain accepted link to the end of the vector
      auto temp       = *chain_.rbegin();
      temp.iteration_ = jumps_;
      chain_.push_back(temp);
    }

  } while (chain_.size() != length_);
}

}  // namespace niwa::mcmcs
#endif  // USE_AUTODIFF
