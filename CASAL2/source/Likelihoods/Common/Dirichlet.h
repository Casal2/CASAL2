//============================================================================
// Name        : Dirichlet.h
// Author      : C.Marsh
// Date        : 21/07/2015
// Copyright   : Copyright NIWA Science ©2009 - www.niwa.co.nz
// Description :
//============================================================================

// Notes for me to get back to- I have copied a combination of Alistair's SPM dirichlet code and Scotts Multinomial code
// I have taken out the override final statement off the Get Initial

#ifndef  LIKELIHOODS_DIRICHLET_H_
#define  LIKELIHOODS_DIRICHLET_H_

// Headers
#include "../../Likelihoods/Likelihood.h"

// Namespaces
namespace niwa {
namespace likelihoods {
/**
 * Class definition
 */
class Dirichlet : public niwa::Likelihood {
public:
  // Methods
	Dirichlet(shared_ptr<Model> model) : Likelihood(model) { };
  virtual                     ~Dirichlet() = default;
  void                        DoValidate() override final { };
  Double                      AdjustErrorValue(const Double process_error, const Double error_value) override final;
  void                        SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) override final;
  Double                      GetInitialScore(map<unsigned, vector<observations::Comparison> >& comparisons, unsigned year) override final;
  void                        GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) override final;
};

} /* namespace likelihoods */
} /* namespace niwa */
#endif /* LIKELIHOODS_DIRICHLET_H_ */
