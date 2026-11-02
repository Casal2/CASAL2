/**
 * @file HamiltonianMonteCarlo.h
 * @author Scott Rasmussen (scott@zaita.com)
 * @brief Implementaton of the Hamiltonian Monte Carlo method
 * @version 0.1
 * @date 2021-04-24
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef SOURCE_MCMCS_COMMON_HAMILTONIANMONTECARLO_H_
#define SOURCE_MCMCS_COMMON_HAMILTONIANMONTECARLO_H_
#ifndef USE_AUTODIFF

// headers
#include "../../Estimates/Estimate.h"
#include "../../MCMCs/MCMC.h"

// namespaces
namespace niwa::mcmcs {

/**
 * @brief Hamiltonian Monte Carlo class implementation
 *
 */
class HamiltonianMonteCarlo : public MCMC {
public:
  // methods
  HamiltonianMonteCarlo(shared_ptr<Model> model);
  virtual ~HamiltonianMonteCarlo() = default;
  void DoExecute(shared_ptr<ThreadPool> thread_pool) final;

protected:
  // methods
  void DoValidate() final;
  void DoBuild() final {};

  /**
   * @brief Compute the squared Euclidean norm of a vector
   *
   * Used for computing kinetic energy and Mahalanobis distance.
   * Result is the sum of squared elements: sum(x_i^2)
   *
   * @param target Vector to compute norm of
   * @return double The squared norm value
   */
  virtual double norm2(const std::vector<double>& target);

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
  virtual double logDensity(const std::vector<double>& x, const std::vector<double>& mean, const std::vector<std::vector<double>>& cov_l, double log_det, double constant);

  /**
   * @brief Compute the kinetic energy from momentum
   *
   * In HMC, kinetic energy is defined as K(p) = 0.5 * p^T * M^{-1} * p
   * With identity mass matrix (M = I), this simplifies to K(p) = 0.5 * ||p||^2
   *
   * @param momentum The momentum vector p
   * @return double The kinetic energy value
   */
  virtual double computeKineticEnergy(const std::vector<double>& momentum);

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
  virtual double computeHamiltonian(double potential_energy, double kinetic_energy);

  /**
   * @brief Perform a half momentum update step of leapfrog integration
   *
   * Updates momentum using: p = p - (dt/2) * grad(U)
   *
   * @param momentum Momentum vector p (modified in place)
   * @param gradient Gradient of potential energy at current position
   * @param delta Time step size (leapfrog_delta_), halved internally
   */
  virtual void leapfrogHalfMomentumStep(std::vector<double>& momentum, const std::vector<double>& gradient, double delta);

  /**
   * @brief Perform the full position update step of leapfrog integration
   *
   * Updates position using: q = q + dt * p
   * This operates in scaled (unbounded) space.
   *
   * @param position Position vector q in scaled space (modified in place)
   * @param momentum Momentum vector p
   * @param delta Time step size (leapfrog_delta_)
   */
  virtual void leapfrogFullPositionStep(std::vector<double>& position, const std::vector<double>& momentum, double delta);

  /**
   * @brief Compute the Metropolis-Hastings acceptance probability
   *
   * The acceptance probability for HMC is: alpha = min(1, exp(-deltaH))
   * where deltaH = H_proposed - H_current. A non-finite deltaH gives 0.
   *
   * @param H_current Current (initial) Hamiltonian
   * @param H_proposed Proposed (final) Hamiltonian
   * @return double Acceptance probability in range [0, 1]
   */
  virtual double computeAcceptanceProbability(double H_current, double H_proposed);

  /**
   * @brief Sample fresh momentum from standard normal distribution N(0, I)
   *
   * In HMC, momentum must be independently sampled at the start of each
   * trajectory. This ensures detailed balance and proper exploration.
   *
   * @param size The dimension of the momentum vector
   * @return std::vector<double> Fresh momentum samples from N(0, I)
   */
  virtual std::vector<double> sampleMomentum(size_t size);

  /**
   * @brief Compute the log Jacobian of the bounded-to-unbounded transformation
   *
   * When transforming from bounded [lower, upper] to unbounded (-inf, inf) space
   * using tan transformation, we need to correct the potential energy by the
   * log absolute Jacobian determinant: log|dq/dq*| = sum_i log|dq_i/dq*_i|
   *
   * For the arctan transformation: q = ((atan(q*)/PI) + 0.5) * (max - min) + min
   * The Jacobian is: dq/dq* = (max - min) / (PI * (1 + q*^2))
   * Log Jacobian: log(max - min) - log(PI) - log(1 + q*^2)
   *
   * @param position_unbounded Position in unbounded space q*
   * @return double The log Jacobian correction (sum over all dimensions)
   */
  virtual double computeLogJacobian(const std::vector<double>& position_unbounded);

  /**
   * @brief Compute the gradient of computeLogJacobian() with respect to q*
   *
   * @param position_unbounded Position in unbounded space q*
   * @return std::vector<double> -2q* / (1 + q*^2) for each dimension
   */
  virtual std::vector<double> computeLogJacobianGradient(const std::vector<double>& position_unbounded);

  /**
   * @brief Set the elements of target that belong to an mcmc_fixed estimate to zero
   *
   * @param target Momentum or gradient vector (modified in place)
   */
  virtual void zeroFixedDimensions(std::vector<double>& target);

  /**
   * @brief Check that every free estimate starts somewhere the sampler can move it from
   *
   * @param position_unbounded Starting position in unbounded space q*
   */
  virtual void checkStartingPosition(const std::vector<double>& position_unbounded);

  /**
   * @brief Scale position values from bounded to unbounded space
   *
   * Uses arctan transformation to map [lower, upper] bounds to (-inf, inf)
   *
   * @param position Position vector to scale (modified in place)
   */
  virtual void scalePosition(std::vector<double>& position);

  /**
   * @brief Unscale position values from unbounded back to bounded space
   *
   * Uses inverse arctan transformation to map (-inf, inf) to [lower, upper] bounds
   *
   * @param position Position vector to unscale (modified in place)
   */
  virtual void unscalePosition(std::vector<double>& position);

  // members
  unsigned       leapfrog_steps_     = 0;
  double         leapfrog_delta_     = 0;
  double         gradient_step_size_ = 0.0;
  vector<double> estimate_lower_bounds_;
  vector<double> estimate_upper_bounds_;

};  // class HamiltonianMonteCarlo
}  // namespace niwa::mcmcs

#endif  // USE_AUTODIFF
#endif  // SOURCE_MCMCS_COMMON_HAMILTONIANMONTECARLO_H_
