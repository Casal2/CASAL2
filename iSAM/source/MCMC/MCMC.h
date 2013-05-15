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
#include <boost/shared_ptr.hpp>

#include "BaseClasses/Object.h"
#include "Minimisers/Minimiser.h"

// namespaces
namespace isam {

namespace ublas = boost::numeric::ublas;
using boost::shared_ptr;

/**
 * Struct definition for a chain link
 */
namespace mcmc {
struct ChainLink {
  unsigned        iteration_;
  double          score_;
  double          likelihood_;
  double          prior_;
  double          penalty_;
  double          acceptance_rate_;
  double          acceptance_rate_since_adapt_;
  double          step_size_;
  vector<double>  values_;
};
}

/**
 * Class definition
 */
class MCMC : public isam::base::Object {
public:
  // Methods
  static shared_ptr<MCMC>     Instance();
  virtual                     ~MCMC() = default;
  void                        Validate();
  void                        Build();
  void                        Execute();

  // Getters/Setters
  const vector<mcmc::ChainLink>&  chain() const { return chain_; }

private:
  // methods
  MCMC();
  void                        BuildCovarianceMatrix();
  bool                        DoCholeskyDecmposition();
  void                        GenerateRandomStart();
  void                        FillMultivariateNormal(double step_size);
  void                        FillMultivariateT(double step_size);
  void                        UpdateStepSize();
  void                        GenerateNewCandidates();

  // members
  double                      start_;
  unsigned                    length_;
  unsigned                    keep_;
  unsigned                    estimate_count_;
  unsigned                    jumps_;
  unsigned                    successful_jumps_;
  unsigned                    jumps_since_adapt_;
  bool                        last_item_;
  unsigned                    successful_jumps_since_adapt_;
  double                      max_correlation_;
  string                      correlation_method_;
  double                      correlation_diff_;
  double                      step_size_;
  string                      proposal_distribution_;
  unsigned                    df_;
//  ublas::matrix<double>       original_covariance_;
  ublas::matrix<double>       covariance_matrix_;
  ublas::matrix<double>       covariance_matrix_lt;
  vector<double>              candidates_;
  vector<bool>                is_enabled_estimate_;
  vector<mcmc::ChainLink>     chain_;
  vector<unsigned>            adapt_step_size_;
  MinimiserPtr                minimiser_;
  vector<string>              estimate_labels_;
};

// Typdef
typedef boost::shared_ptr<MCMC> MCMCPtr;

} /* namespace isam */
#endif /* MCMC_H_ */
