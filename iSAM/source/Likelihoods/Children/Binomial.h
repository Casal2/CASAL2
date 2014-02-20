/**
 * @file Binomial.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 25/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef LIKELIHOODS_BINOMIAL_H_
#define LIKELIHOODS_BINOMIAL_H_

// Headers
#include "Likelihoods/Likelihood.h"

// Namespaces
namespace isam {
namespace likelihoods {

/**
 * Class definition
 */
class Binomial : public isam::Likelihood {
public:
  // Methods
  Binomial() = default;
  virtual                     ~Binomial() = default;
  Double                      AdjustErrorValue(const Double process_error, const Double error_value) override final;
  void                        GetResult(vector<Double> &scores, const vector<Double> &expecteds, const vector<Double> &observeds,
                                const vector<Double> &error_values, const vector<Double> &process_errors, const Double delta) override final;
  void                        SimulateObserved(const vector<string> &keys, vector<Double> &observeds, const vector<Double> &expecteds,
                                const vector<Double> &error_values, const vector<Double> &process_errors, const Double delta) override final;

  void                        SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) override final;
  void                        GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) override final;
};

} /* namespace likelihoods */
} /* namespace isam */
#endif /* LIKELIHOODS_BINOMIAL_H_ */
