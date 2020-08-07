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
 * In the beta, the a,b, are the bounds where the curve ends, mu is the "peak" and the sigma gives the rate of change
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef ESTIMATES_BETA_H_
#define ESTIMATES_BETA_H_

// Headers
#include "Estimates/Estimate.h"

// Namespaces
namespace niwa {
namespace estimates {

/**
 * Class Definition
 */
class Beta : public niwa::Estimate {
public:
  // Methods
  Beta() = delete;
  explicit Beta(Model* model);
  virtual                     ~Beta() = default;
  void                        DoValidate() override final;
  Double                      GetScore() override final;

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

} /* namespace estimates */
} /* namespace niwa */
#endif /* ESTIMATES_BETA_H_ */
