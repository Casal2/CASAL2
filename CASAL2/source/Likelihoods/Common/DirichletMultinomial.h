//============================================================================
// Name        : DirichletMultinomial.h
// Author      : C.Marsh
// Date        : 11/05/22
// Copyright   : Copyright Casal2 Project 2024 - https://github.com/Casal2/
// Description :
//============================================================================

// DirichletMultinomial  error distribution

#ifndef LIKELIHOODS_DIRICHLET_MULTINOMIAL_H_
#define LIKELIHOODS_DIRICHLET_MULTINOMIAL_H_

// Headers
#include "../../Likelihoods/Likelihood.h"
// Namespaces
namespace niwa {
namespace likelihoods {

/**
 * Class definition
 */
class DirichletMultinomial : public niwa::Likelihood {
public:
  // Methods
  DirichletMultinomial(shared_ptr<Model> model);
  virtual ~DirichletMultinomial() = default;
  void   DoValidate() override final;
  void   DoVerify(shared_ptr<Model> model) override final;

  Double AdjustErrorValue(const Double process_error, const Double error_value) override final;
  void   SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) override final;
  Double GetInitialScore(map<unsigned, vector<observations::Comparison> >& comparisons, unsigned year) override final;
  void   GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) override final;

protected:
  Double                      theta_;

};

} /* namespace likelihoods */
} /* namespace niwa */
#endif /* LIKELIHOODS_DIRICHLET_MULTINOMIAL_H_ */
