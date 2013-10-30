/**
 * @file Beta.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
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
#ifndef PRIORS_BETA_H_
#define PRIORS_BETA_H_

// Headers
#include "Priors/Prior.h"

// Namespaces
namespace isam {
namespace priors {

/**
 * Class Definition
 */
class Beta : public isam::Prior {
public:
  // Methods
  Beta();
  virtual                     ~Beta() = default;
  void                        DoValidate() override final;
  Double                      GetScore(Double param) override final;

private:
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

} /* namespace priors */
} /* namespace isam */
#endif /* PRIORS_BETA_H_ */
