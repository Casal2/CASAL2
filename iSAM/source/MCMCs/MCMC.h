/**
 * @file MCMC.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 8/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Markov Chain Monte Carlo
 */

#ifndef MCMC_H_
#define MCMC_H_

// headers
#include <boost/numeric/ublas/matrix.hpp>

#include "BaseClasses/Object.h"
#include "Minimisers/Minimiser.h"

// namespaces
namespace niwa {

namespace ublas = boost::numeric::ublas;
using std::shared_ptr;

/**
 * Struct definition for a chain link
 */
namespace mcmc {
struct ChainLink {
  unsigned        iteration_;
  Double          score_;
  Double          likelihood_;
  Double          prior_;
  Double          penalty_;
  Double          additional_priors_;
  Double          acceptance_rate_;
  Double          acceptance_rate_since_adapt_;
  Double          step_size_;
  vector<Double>  values_;
};
}

/**
 * Class definition
 */
class MCMC : public niwa::base::Object {
public:
  // Methods
  MCMC();
  virtual                     ~MCMC() = default;
  void                        Validate();
  void                        Build();
  void                        Reset() { };
  virtual void                Execute() = 0;

  // Getters/Setters
  const vector<mcmc::ChainLink>&  chain() const { return chain_; }
  bool                        active() const { return active_; }

  // accessors
  vector<string>              GetEstimateLabel() const {return estimate_labels_;}
  ublas::matrix<Double>&      GetCovarianceMatrix() {return covariance_matrix_;}
protected:
  // pure virtual methods
  virtual void                DoValidate() = 0;
  virtual void                DoBuild() = 0;

  // methods
  void                        BuildCovarianceMatrix();
  bool                        DoCholeskyDecmposition();
  void                        GenerateRandomStart();
  void                        FillMultivariateNormal(Double step_size);
  void                        FillMultivariateT(Double step_size);
  void                        UpdateStepSize();
  void                        GenerateNewCandidates();

  // members
  Double                      start_ = 0;
  unsigned                    length_ = 0;
  unsigned                    keep_ = 0;
  unsigned                    estimate_count_ = 0;
  unsigned                    jumps_ = 0;
  unsigned                    successful_jumps_ = 0;
  unsigned                    jumps_since_adapt_ = 0;
  bool                        last_item_ = false;
  unsigned                    successful_jumps_since_adapt_ = 0;
  Double                      max_correlation_ = 0;
  string                      correlation_method_ = "";
  Double                      correlation_diff_ = 0;
  Double                      step_size_ = 0;
  string                      proposal_distribution_ = "";
  unsigned                    df_ = 0;
  ublas::matrix<Double>       covariance_matrix_;
  ublas::matrix<Double>       covariance_matrix_lt;
  vector<Double>              candidates_;
  vector<bool>                is_enabled_estimate_;
  vector<mcmc::ChainLink>     chain_;
  vector<unsigned>            adapt_step_size_;
  MinimiserPtr                minimiser_;
  vector<string>              estimate_labels_;
  bool                        active_;
};

// Typdef
typedef std::shared_ptr<MCMC> MCMCPtr;

} /* namespace niwa */
#endif /* MCMC_H_ */
