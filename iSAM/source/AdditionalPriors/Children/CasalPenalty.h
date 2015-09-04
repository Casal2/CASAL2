/**
 * @file CasalPenalty.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * TODO: Add documentation
 */
#ifndef ADDITIONALPRIORS_CASAL_PENALTY_H_
#define ADDITIONALPRIORS_CASAL_PENALTY_H_

// headers
#include "AdditionalPriors/AdditionalPrior.h"

// namespaces
namespace niwa {
namespace additionalpriors {

// classes
class CasalPenalty : public AdditionalPrior {
public:
  // methods
  CasalPenalty();
  virtual                     ~CasalPenalty() = default;
  void                        DoValidate() override final;

protected:
  // methods
  Double                      ratio_score() override final;
  Double                      difference_score() override final;
  Double                      mean_score() override final;

  // members
  Double                      multiplier_ = 0.0;
  bool                        log_scale_ = 0.0;
z
};

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ADDITIONALPRIORS_CASAL_PENALTY_H_ */
