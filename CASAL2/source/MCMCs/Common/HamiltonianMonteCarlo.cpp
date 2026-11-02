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

#include <algorithm>
#include <cmath>
#include <limits>

#include "../../Estimates/Manager.h"
#include "../../Minimisers/Manager.h"
#include "../../Model/Model.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"
#include "../../Reports/Manager.h"
#include "../../Utilities/Gradient.h"
#include "../../Utilities/Math.h"
#include "../../Utilities/RandomNumberGenerator.h"

// namespaces
namespace niwa::mcmcs {

namespace math = niwa::utilities::math;
using math::PI;

// A parameter scaled beyond this is within ~3e-7 of a bound (as a fraction of its range). Finite difference
// gradients and leapfrog steps cannot move it from there
constexpr double kMaxStartingScaledValue = 1e6;

/**
 * @brief norm2. Return cumulative squard values of
 * the vector
 *
 * @param target
 * @return double
 */
double HamiltonianMonteCarlo::norm2(const std::vector<double>& target) {
  double result = 0.0;
  for (auto d : target) result += d * d;
  return result;
}

/**
 * @brief Calculate the log density of a multivariate normal distribution
 *
 * @param x The point at which to evaluate the density
 * @param mean The mean vector of the distribution
 * @param cov_l The Cholesky decomposition (lower triangular) of the covariance matrix
 * @param log_det The log determinant of the covariance matrix
 * @param constant The normalization constant (-n/2 * log(2*pi))
 * @return double The log density value
 */
double HamiltonianMonteCarlo::logDensity(const std::vector<double>& x, const std::vector<double>& mean, const std::vector<std::vector<double>>& cov_l, double log_det,
                                         double constant) {
  std::vector<double> diff(x.size());
  for (size_t i = 0; i < x.size(); ++i) {
    diff[i] = mean[i] - x[i];
  }

  // Forward solve: L * y = diff
  // This gives y = L^(-1) * diff
  // The Mahalanobis distance is diff^T * Σ^(-1) * diff = y^T * y = norm2(y)
  std::vector<double> y(diff.size(), 0.0);
  for (size_t i = 0; i < diff.size(); ++i) {
    double sum = diff[i];
    for (size_t j = 0; j < i; ++j) {
      sum -= cov_l[i][j] * y[j];
    }
    y[i] = sum / cov_l[i][i];
  }

  double result = constant - 0.5 * log_det - 0.5 * norm2(y);
  return result;
}

/**
 * @brief Compute the kinetic energy from momentum
 *
 * In HMC, kinetic energy is defined as K(p) = 0.5 * p^T * M^{-1} * p
 * With identity mass matrix (M = I), this simplifies to K(p) = 0.5 * ||p||^2
 *
 * @param momentum The momentum vector p
 * @return double The kinetic energy value
 */
double HamiltonianMonteCarlo::computeKineticEnergy(const std::vector<double>& momentum) {
  return 0.5 * norm2(momentum);
}

/**
 * @brief Compute the total Hamiltonian (total energy)
 *
 * The Hamiltonian H(q,p) = U(q) + K(p) represents total energy
 * and should be conserved during perfect leapfrog integration.
 *
 * @param potential_energy The potential energy U(q)
 * @param kinetic_energy The kinetic energy K(p)
 * @return double The total Hamiltonian value
 */
double HamiltonianMonteCarlo::computeHamiltonian(double potential_energy, double kinetic_energy) {
  return potential_energy + kinetic_energy;
}

/**
 * @brief Perform a half momentum update step of leapfrog integration
 *
 * Updates momentum using: p = p - (dt/2) * gradient(U)
 * The gradient points towards higher potential, so we subtract.
 *
 * @param momentum Momentum vector p (modified in place)
 * @param gradient Gradient of potential energy at current position
 * @param delta Time step size (full step, will be halved internally)
 */
void HamiltonianMonteCarlo::leapfrogHalfMomentumStep(std::vector<double>& momentum, const std::vector<double>& gradient, double delta) {
  double delta_half = delta / 2.0;
  for (size_t i = 0; i < momentum.size(); ++i) {
    momentum[i] = momentum[i] - delta_half * gradient[i];
  }
}

/**
 * @brief Perform the full position update step of leapfrog integration
 *
 * Updates position using: q = q + dt * p
 * Position should be in scaled (unbounded) space when calling this.
 *
 * @param position Position vector q in scaled space (modified in place)
 * @param momentum Momentum vector p
 * @param delta Time step size (leapfrog_delta_)
 */
void HamiltonianMonteCarlo::leapfrogFullPositionStep(std::vector<double>& position, const std::vector<double>& momentum, double delta) {
  for (size_t i = 0; i < position.size(); ++i) {
    position[i] = position[i] + delta * momentum[i];
  }
}

/**
 * @brief Compute the Metropolis-Hastings acceptance probability
 *
 * The acceptance probability for HMC is: alpha = min(1, exp(-deltaH))
 * where deltaH = H_proposed - H_current
 *
 * A non-finite deltaH is a divergent trajectory and is never accepted. This must be
 * checked explicitly as std::min(1.0, NaN) returns 1.0.
 *
 * @param H_current Current (initial) Hamiltonian
 * @param H_proposed Proposed (final) Hamiltonian
 * @return double Acceptance probability in range [0, 1]
 */
double HamiltonianMonteCarlo::computeAcceptanceProbability(double H_current, double H_proposed) {
  double delta_H = H_proposed - H_current;
  if (!std::isfinite(delta_H)) {
    LOG_MEDIUM() << "Divergent trajectory (H_current: " << H_current << ", H_proposed: " << H_proposed << ")";
    return 0.0;
  }

  return std::min(1.0, std::exp(-delta_H));
}

/**
 * @brief Sample fresh momentum from standard normal distribution N(0, I)
 *
 * In HMC, momentum must be independently sampled at the start of each
 * trajectory. This ensures detailed balance and proper exploration.
 *
 * @param size The dimension of the momentum vector
 * @return std::vector<double> Fresh momentum samples from N(0, I)
 */
std::vector<double> HamiltonianMonteCarlo::sampleMomentum(size_t size) {
  utilities::RandomNumberGenerator& rng = utilities::RandomNumberGenerator::Instance();
  std::vector<double>               momentum(size);
  for (size_t i = 0; i < size; ++i) {
    momentum[i] = rng.normal();
  }
  return momentum;
}

/**
 * @brief Compute the log Jacobian of the bounded-to-unbounded transformation
 *
 * When transforming from bounded [lower, upper] to unbounded (-inf, inf) space
 * using tan transformation, we need to correct the potential energy by the
 * log absolute Jacobian determinant.
 *
 * For the arctan transformation: q = ((atan(q*)/PI) + 0.5) * (max - min) + min
 * The Jacobian is: dq/dq* = (max - min) / (PI * (1 + q*^2))
 * Log Jacobian: log(max - min) - log(PI) - log(1 + q*^2)
 *
 * The sign is positive because we add this correction when computing
 * the transformed potential energy (we're working in q* space).
 *
 * @param position_unbounded Position in unbounded space q*
 * @return double The log Jacobian correction (sum over all dimensions)
 */
double HamiltonianMonteCarlo::computeLogJacobian(const std::vector<double>& position_unbounded) {
  double log_jacobian = 0.0;
  for (size_t i = 0; i < position_unbounded.size(); ++i) {
    double range     = estimate_upper_bounds_[i] - estimate_lower_bounds_[i];
    double q_star    = position_unbounded[i];
    double q_star_sq = q_star * q_star;
    // log|dq/dq*| = log(range/PI) - log(1 + q*^2)
    log_jacobian += std::log(range / PI) - std::log(1.0 + q_star_sq);
  }
  return log_jacobian;
}

/**
 * @brief Compute the gradient of computeLogJacobian() with respect to q*
 *
 * d/dq* [log(range/PI) - log(1 + q*^2)] = -2q* / (1 + q*^2)
 *
 * The potential energy is U* = U - log|J|, so this is subtracted from the gradient of U.
 *
 * @param position_unbounded Position in unbounded space q*
 * @return std::vector<double> The gradient of the log Jacobian for each dimension
 */
std::vector<double> HamiltonianMonteCarlo::computeLogJacobianGradient(const std::vector<double>& position_unbounded) {
  std::vector<double> gradient(position_unbounded.size(), 0.0);
  for (size_t i = 0; i < position_unbounded.size(); ++i) {
    double q_star = position_unbounded[i];
    gradient[i]   = -2.0 * q_star / (1.0 + q_star * q_star);
  }
  return gradient;
}

/**
 * @brief Set the elements of target that belong to an mcmc_fixed estimate to zero
 *
 * With no momentum and no gradient, those dimensions never move during leapfrog integration
 *
 * @param target Momentum or gradient vector (modified in place)
 */
void HamiltonianMonteCarlo::zeroFixedDimensions(std::vector<double>& target) {
  for (size_t i = 0; i < target.size(); ++i) {
    if (estimates_[i]->mcmc_fixed())
      target[i] = 0.0;
  }
}

/**
 * @brief Check that every free estimate starts somewhere the sampler can move it from
 *
 * Near a bound the scaled value is so large that neither a leapfrog step nor the finite
 * difference step changes it, so the estimate would silently stay at the bound.
 *
 * @param position_unbounded Starting position in unbounded space q*
 */
void HamiltonianMonteCarlo::checkStartingPosition(const std::vector<double>& position_unbounded) {
  for (size_t i = 0; i < position_unbounded.size(); ++i) {
    if (!estimates_[i]->mcmc_fixed() && !(std::abs(position_unbounded[i]) <= kMaxStartingScaledValue)) {
      LOG_FATAL() << "The estimate " << estimates_[i]->parameter() << " is at (or outside) a bound, so the MCMC of type " << PARAM_HAMILTONIAN << " cannot move it. Set "
                  << PARAM_START << " greater than zero and " << PARAM_PARAMETER_AT_BOUND << " to true to start it away from the bound";
    }
  }
}

/**
 * @brief Scale position values from bounded to unbounded space
 *
 * Uses arctan transformation to map [lower, upper] bounds to (-inf, inf)
 *
 * @param position Position vector to scale (modified in place)
 */
void HamiltonianMonteCarlo::scalePosition(std::vector<double>& position) {
  math::scale_vector(position, estimate_lower_bounds_, estimate_upper_bounds_);
}

/**
 * @brief Unscale position values from unbounded back to bounded space
 *
 * Uses inverse arctan transformation to map (-inf, inf) to [lower, upper] bounds
 *
 * @param position Position vector to unscale (modified in place)
 */
void HamiltonianMonteCarlo::unscalePosition(std::vector<double>& position) {
  math::unscale_vector(position, estimate_lower_bounds_, estimate_upper_bounds_);
}

/**
 * @brief Construct a new Hamiltonian Monte Carlo:: Hamiltonian Monte Carlo object
 *
 * @param model
 */
HamiltonianMonteCarlo::HamiltonianMonteCarlo(shared_ptr<Model> model) : MCMC(model) {
  parameters_.Bind<unsigned>(PARAM_LEAPFROG_STEPS, &leapfrog_steps_, "Number of leapfrog steps")->set_default_value(10u);
  parameters_.Bind<double>(PARAM_LEAPFROG_DELTA, &leapfrog_delta_, "Amount to leapfrog per step")->set_default_value(0.01);
  parameters_.Bind<double>(PARAM_GRADIENT_STEP_SIZE, &gradient_step_size_, "Step size to use when calculating gradient")->set_default_value(1e-6);

  type_ = PARAM_HAMILTONIAN;
}

/**
 *
 */
void HamiltonianMonteCarlo::DoValidate() {
  parameters_.Validate(PARAM_LEAPFROG_STEPS)->GreaterThan(0u);
  parameters_.Validate(PARAM_LEAPFROG_DELTA)->GreaterThan(0.0);
  parameters_.Validate(PARAM_GRADIENT_STEP_SIZE)->GreaterThan(1e-13);

  for (const char* parameter : {PARAM_STEP_SIZE, PARAM_ADAPT_STEPSIZE_AT, PARAM_ADAPT_COVARIANCE_AT, PARAM_PROPOSAL_DISTRIBUTION, PARAM_DF}) {
    if (parameters_.Get(parameter)->has_been_defined())
      LOG_WARNING() << parameters_.location(parameter) << " is not used by the MCMC of type " << PARAM_HAMILTONIAN << " and will be ignored";
  }
}

/**
 * @brief Execute the HMC algorithm using the thread pool for gradient calculations
 *
 * The algorithm works entirely in unbounded (transformed) space:
 * - Position q* is in unbounded space via tan transformation
 * - Momentum p is sampled fresh from N(0, I) each iteration
 * - Potential energy includes Jacobian correction for the transformation
 *
 * As with the other MCMC types, length is the total number of iterations, and
 * every keep'th iteration is stored in the chain and sent to the reports.
 *
 * @param thread_pool Pool of threads to use when computing gradients
 */
void HamiltonianMonteCarlo::DoExecute(shared_ptr<ThreadPool> thread_pool) {
  LOG_MEDIUM() << "Starting Hamiltonian Monte Carlo";
  ObjectiveFunction&                obj_function = model()->objective_function();
  utilities::RandomNumberGenerator& rng          = utilities::RandomNumberGenerator::Instance();

  estimate_lower_bounds_.resize(estimates_.size(), 0.0);
  estimate_upper_bounds_.resize(estimates_.size(), 0.0);
  for (unsigned i = 0; i < estimates_.size(); ++i) {
    estimate_lower_bounds_[i] = estimates_[i]->lower_bound();
    estimate_upper_bounds_[i] = estimates_[i]->upper_bound();
  }

  // The current state of the chain. This is not always the last link as keep can thin the chain
  mcmc::ChainLink      current_link   = chain_.back();
  const vector<double> start_values   = current_link.values_;
  vector<double>       current_q_star = start_values;
  scalePosition(current_q_star);

  checkStartingPosition(current_q_star);

  // Set the estimates and run the model at q*, storing the results in link. The first model in the thread
  // pool is model(), so the objective must be read here before the next gradient calculation overwrites it.
  // Returns the potential energy in unbounded space, U* = U - log|J|
  auto evaluate = [this, &obj_function, &start_values](const vector<double>& q_star, mcmc::ChainLink& link) {
    vector<double> q_bounded = q_star;
    unscalePosition(q_bounded);
    for (unsigned i = 0; i < q_bounded.size(); ++i) {
      if (estimates_[i]->mcmc_fixed())
        q_bounded[i] = start_values[i];
      estimates_[i]->set_value(q_bounded[i]);
    }

    model()->FullIteration();
    obj_function.CalculateScore();

    link.score_             = obj_function.score();
    link.likelihood_        = obj_function.likelihoods();
    link.prior_             = obj_function.priors();
    link.penalty_           = obj_function.penalties();
    link.additional_priors_ = obj_function.additional_priors();
    link.jacobians_         = obj_function.jacobians();
    link.values_            = q_bounded;

    return link.score_ - computeLogJacobian(q_star);
  };

  // Gradient of the potential energy in unbounded space. The thread pool returns raw objective
  // scores so raw_score is the baseline and the Jacobian's gradient is applied afterwards.
  auto calculate_gradient = [this, &thread_pool](const vector<double>& q_star, double raw_score) {
    vector<double> gradient          = utilities::gradient::Calculate(thread_pool, q_star, estimate_lower_bounds_, estimate_upper_bounds_, gradient_step_size_, raw_score, true);
    vector<double> jacobian_gradient = computeLogJacobianGradient(q_star);
    for (size_t i = 0; i < gradient.size(); ++i) gradient[i] -= jacobian_gradient[i];
    zeroFixedDimensions(gradient);
    return gradient;
  };

  double current_potential = evaluate(current_q_star, current_link);
  if (!std::isfinite(current_potential))
    LOG_FATAL() << "The objective function score at the start of the MCMC is not finite (" << current_link.score_ << ")";
  vector<double> current_gradient = calculate_gradient(current_q_star, current_link.score_);

  while (jumps_ < length_) {
    /**
     * @brief Hamiltonian Monte Carlo uses leapfrog integration in unbounded space.
     *
     * The algorithm:
     * 1. Sample fresh momentum p from N(0, I)
     * 2. Perform L leapfrog steps (all in unbounded space)
     * 3. Accept/reject based on Hamiltonian change
     *
     * The gradient at the end of a step is the gradient at the start of the next, and the
     * position, potential and gradient of an accepted proposal start the next iteration.
     */
    vector<double> q_star = current_q_star;

    // Sample fresh momentum from N(0, I) - CRITICAL for correct HMC
    vector<double> p = sampleMomentum(estimates_.size());
    zeroFixedDimensions(p);
    double K0 = computeKineticEnergy(p);

    mcmc::ChainLink proposed_link      = current_link;
    double          proposed_potential = std::numeric_limits<double>::infinity();
    vector<double>  gradient           = current_gradient;

    for (unsigned i = 0; i < leapfrog_steps_; ++i) {
      leapfrogHalfMomentumStep(p, gradient, leapfrog_delta_);
      leapfrogFullPositionStep(q_star, p, leapfrog_delta_);

      // A divergent trajectory is rejected without giving the model non-finite parameters
      if (!std::all_of(q_star.begin(), q_star.end(), [](double value) { return std::isfinite(value); })) {
        proposed_potential = std::numeric_limits<double>::infinity();
        break;
      }

      proposed_potential = evaluate(q_star, proposed_link);
      if (!std::isfinite(proposed_potential))
        break;

      gradient = calculate_gradient(q_star, proposed_link.score_);
      leapfrogHalfMomentumStep(p, gradient, leapfrog_delta_);
    }

    double H0 = computeHamiltonian(current_potential, K0);
    double H  = computeHamiltonian(proposed_potential, computeKineticEnergy(p));

    // Metropolis acceptance criterion
    double accept_prob = computeAcceptanceProbability(H0, H);
    double rng_uniform = rng.uniform();
    double delta_H     = H - H0;

    jumps_++;
    jumps_since_adapt_++;

    // Check if we accept this jump
    if (rng_uniform < accept_prob) {
      LOG_MEDIUM() << "Accepting Jump (iteration: " << jumps_ << ", accept_prob: " << accept_prob << ", rng: " << rng_uniform << ", delta_H: " << delta_H << ")";
      successful_jumps_++;
      successful_jumps_since_adapt_++;

      current_link      = proposed_link;
      current_q_star    = q_star;
      current_potential = proposed_potential;
      current_gradient  = gradient;

    } else {
      LOG_MEDIUM() << "Rejecting Jump (iteration: " << jumps_ << ", accept_prob: " << accept_prob << ", rng: " << rng_uniform << ", delta_H: " << delta_H << ")";
    }

    if (jumps_ % keep_ == 0) {
      mcmc_state_                               = (jumps_ > burn_in_) ? PARAM_MCMC : PARAM_BURN_IN;
      current_link.iteration_                   = jumps_;
      current_link.mcmc_state_                  = mcmc_state_;
      current_link.acceptance_rate_             = double(successful_jumps_) / double(jumps_);
      current_link.acceptance_rate_since_adapt_ = double(successful_jumps_since_adapt_) / double(jumps_since_adapt_);
      current_link.step_size_                   = step_size_;

      chain_.push_back(current_link);
      model()->managers()->report()->Execute(model(), State::kIterationComplete);
    }
  }
}

}  // namespace niwa::mcmcs
#endif  // USE_AUTODIFF
