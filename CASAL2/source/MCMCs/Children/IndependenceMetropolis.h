/**
 * @file IndependenceMetropolis.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/05/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_MCMCS_CHILDREN_INDEPENDENCEMETROPOLIS_H_
#define SOURCE_MCMCS_CHILDREN_INDEPENDENCEMETROPOLIS_H_

// headers
#include "MCMCs/MCMC.h"

// namespaces
namespace niwa {
namespace mcmcs {

/**
 * Class definition
 */
class IndependenceMetropolis : public MCMC {
public:
  // methods
  IndependenceMetropolis(Model* model);
  virtual                     ~IndependenceMetropolis() = default;
  void                        DoExecute() override final;

protected:
  // methods
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        BuildCovarianceMatrix();
  bool                        DoCholeskyDecmposition();
  void                        GenerateRandomStart();
  void                        FillMultivariateNormal(Double step_size);
  void                        FillMultivariateT(Double step_size);
  void                        UpdateStepSize();
  void                        GenerateNewCandidates();

  // members
  Double                      start_ = 0;
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
  ublas::matrix<Double>       covariance_matrix_lt;
  vector<Double>              candidates_;
  vector<bool>                is_enabled_estimate_;
  vector<unsigned>            adapt_step_size_;
  Minimiser*                  minimiser_;
  vector<string>              estimate_labels_;
};

} /* namespace mcmcs */
} /* namespace niwa */

#endif /* SOURCE_MCMCS_CHILDREN_INDEPENDENCEMETROPOLIS_H_ */
