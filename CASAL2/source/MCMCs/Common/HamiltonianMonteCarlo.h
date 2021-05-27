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
  void DoValidate() final{};
  void DoBuild() final{};
  void GeneratedNewScaledCandidates();

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
