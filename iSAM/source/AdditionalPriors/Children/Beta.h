/**
 * @file Beta.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef ADDITIONALPRIORS_BETA_H_
#define ADDITIONALPRIORS_BETA_H_

// headers
#include "AdditionalPriors/AdditionalPrior.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 *
 */
class Beta : public AdditionalPrior {
public:
  // methods
  Beta();
  virtual                     ~Beta() = default;
  void                        DoValidate() override final;

protected:
  // methods
  Double                      GetRatioScore() override final;
  Double                      GetDifferenceScore() override final;
  Double                      GetMeanScore() override final;

  // Members
  Double                      mu_ = 0.0;
  Double                      sigma_ = 0.0;
  Double                      a_ = 0.0;
  Double                      b_ = 0.0;
  Double                      v_ = 0.0;
  Double                      t_ = 0.0;
  Double                      m_ = 0.0;
  Double                      n_ = 0.0;
};

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ADDITIONALPRIORS_BETA_H_ */
