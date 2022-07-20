/**
 * @file Bernoulli.h
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 */
#ifndef LIKELIHOODS_BERNOULLI_H_
#define LIKELIHOODS_BERNOULLI_H_

// Headers
#include "../../Likelihoods/Likelihood.h"

// Namespaces
namespace niwa {
namespace likelihoods {

/**
 * Class definition
 */
class Bernoulli : public niwa::Likelihood {
public:
  // Methods
  Bernoulli(shared_ptr<Model> model) : Likelihood(model){};
  virtual ~Bernoulli() = default;
  void   DoValidate() override final{};
  Double AdjustErrorValue(const Double process_error, const Double error_value) override final;
  void   SimulateObserved(map<unsigned, vector<observations::Comparison> >& comparisons) override final;
  void   GetScores(map<unsigned, vector<observations::Comparison> >& comparisons) override final;
};

} /* namespace likelihoods */
} /* namespace niwa */
#endif /* LIKELIHOODS_BERNOULLI_H_ */
