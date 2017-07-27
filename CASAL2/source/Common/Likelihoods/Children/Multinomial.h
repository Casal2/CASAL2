/**
 * @file Multinomial.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 25/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef LIKELIHOODS_MULTINOMIAL_H_
#define LIKELIHOODS_MULTINOMIAL_H_

// Headers
#include "Common/Likelihoods/Likelihood.h"

// Namespaces
namespace niwa {
namespace likelihoods {

/**
 * Class definition
 */
class Multinomial : public niwa::Likelihood {
public:
  // Methods
  Multinomial() = default;
  virtual                     ~Multinomial() = default;
  void                        DoValidate() override final { };
  Double                      AdjustErrorValue(const Double process_error, const Double error_value) override final;
  void                        SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) override final;
  Double                      GetInitialScore(map<unsigned, vector<observations::Comparison> >& comparisons, unsigned year) override final;
  void                        GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) override final;
};

} /* namespace likelihoods */
} /* namespace niwa */
#endif /* LIKELIHOODS_MULTINOMIAL_H_ */
