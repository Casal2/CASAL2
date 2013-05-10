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
namespace isam {

namespace ublas = boost::numeric::ublas;

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
  void                        Validate();
  void                        Build();
  void                        Execute();

private:
  // methods
  MCMC();
  virtual                     ~MCMC() = default;

  // members
  double                      start_;
  unsigned                    length_;
  unsigned                    keep_;
  unsigned                    estimate_cout_;
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
  ublas::matrix<double>       original_covariance_;
  ublas::matrix<double>       covariance_;
  ublas::matrix<double>       covariance_lt;
  vector<double>              candidates_;
  vector<bool>                is_enabled_estimate_;
  mcmc::ChainLink             chain_link_;
  vector<mcmc::ChainLink>     chain_;
  vector<unsigned>            adapt_step_size_;
  MinimiserPtr                minimiser_;
  vector<string>              estimate_labels_;
};

} /* namespace isam */
#endif /* MCMC_H_ */
