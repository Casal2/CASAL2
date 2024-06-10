/**
 * @file Poisson.h
 * @author  C Marsh
 * @version 1.0
 * @date 2023
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef LIKELIHOODS_POISSON_H_
#define LIKELIHOODS_POISSON_H_

// Headers
#include "../../Likelihoods/Likelihood.h"

// Namespaces
namespace niwa {
namespace likelihoods {

/**
 * Class definition
 */
class Poisson  : public niwa::Likelihood {
public:
  // Methods
  Poisson (shared_ptr<Model> model) : Likelihood(model){};
  virtual ~Poisson () = default;
  void   DoValidate() override final{};
  void   GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) override final;
  Double AdjustErrorValue(const Double process_error, const Double error_value) override final;
  void   SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) override final;
};

} /* namespace likelihoods */
} /* namespace niwa */
#endif /* LIKELIHOODS_POISSON_H_ _H_ */
