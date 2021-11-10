/**
 * @file MCMC.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 8/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Markov Chain Monte Carlo
 */

#ifndef MCMC_H_
#define MCMC_H_

// headers
#include <boost/numeric/ublas/matrix.hpp>

#include "../BaseClasses/Object.h"
#include "../Estimates/Estimate.h"
#include "../ThreadPool/ThreadPool.h"

// namespaces
namespace niwa {
namespace ublas = boost::numeric::ublas;

class Minimiser;
class Model;

/**
 * Struct definition for a chain link
 */
namespace mcmc {
struct ChainLink {
  unsigned       iteration_                   = 1;
  string         mcmc_state_                  = "NA";
  double         score_                       = 0.0;
  double         likelihood_                  = 0.0;
  double         prior_                       = 0.0;
  double         penalty_                     = 0.0;
  double         additional_priors_           = 0.0;
  double         jacobians_                   = 0.0;
  double         acceptance_rate_             = 0.0;
  double         acceptance_rate_since_adapt_ = 0.0;
  double         step_size_                   = 0.0;
  vector<double> values_;
  vector<double> scaled_values_;
};
}  // namespace mcmc

/**
 * Class definition
 */
class MCMC : public niwa::base::Object {
public:
  // Methods
  MCMC() = delete;
  explicit MCMC(shared_ptr<Model> model);
  virtual ~MCMC() = default;
  void Validate();
  void Build();
  void Verify(shared_ptr<Model> model){};
  void Reset(){};
  void Execute(shared_ptr<ThreadPool> thread_pool);

  // accessors/mutators
  vector<mcmc::ChainLink>& chain() { return chain_; }
  bool                     active() const { return active_; }
  ublas::matrix<double>&   covariance_matrix() { return covariance_matrix_; }
  void                     set_starting_iteration(unsigned value) { starting_iteration_ = value; }
  void                     set_successful_jumps(unsigned value) { successful_jumps_ = value; }
  void                     set_step_size(double value) { step_size_ = value; }
  void                     set_acceptance_rate(double value) { acceptance_rate_ = value; }
  void                     set_acceptance_rate_from_last_adapt(double value) { acceptance_rate_since_last_adapt_ = value; }
  bool                     recalculate_covariance() const { return recalculate_covariance_; }

protected:
  // pure virtual methods
  virtual void DoValidate()                                  = 0;
  virtual void DoBuild()                                     = 0;
  virtual void DoExecute(shared_ptr<ThreadPool> thread_pool) = 0;

  // methods
  void ResumeChain();
  void GenerateRandomStart();
  void GenerateNewCandidates();
  void FillMultivariateNormal(double step_size);
  void FillMultivariateT(double step_size);
  void UpdateStepSize();
  void BuildCovarianceMatrix();
  void UpdateCovarianceMatrix();
  bool DoCholeskyDecomposition();
  bool WithinBounds();

  // members
  shared_ptr<Model>       model_;  // first model in thread pool
  unsigned                length_             = 0;
  unsigned                starting_iteration_ = 0;
  unsigned                burn_in_            = 0;
  string                  mcmc_state_         = "NA";
  bool                    active_             = true;
  unsigned                estimate_count_     = 0;
  unsigned                df_                 = 0;
  double                  start_              = 0;
  ublas::matrix<double>   covariance_matrix_;
  ublas::matrix<double>   covariance_matrix_lt;
  vector<mcmc::ChainLink> chain_;
  bool                    print_default_reports_;
  bool                    recalculate_covariance_           = false;
  double                  step_size_                        = 0.0;
  unsigned                successful_jumps_                 = 0;
  double                  acceptance_rate_                  = 0;
  double                  acceptance_rate_since_last_adapt_ = 0;
  string                  correlation_method_               = "";
  double                  correlation_diff_                 = 0;
  double                  max_correlation_                  = 0;
  string                  proposal_distribution_            = "";
  vector<Estimate*>       estimates_;
  vector<double>          candidates_;
  unsigned                jumps_since_adapt_            = 0;
  unsigned                keep_                         = 0;
  unsigned                jumps_                        = 0;
  bool                    last_item_                    = false;
  unsigned                successful_jumps_since_adapt_ = 0;
  vector<unsigned>        adapt_step_size_              = {0};
  unsigned                adapt_covariance_matrix_      = 0;
  string                  adapt_stepsize_method_;
};

} /* namespace niwa */
#endif /* MCMC_H_ */
