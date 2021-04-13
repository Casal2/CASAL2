/**
 * @file LogNormal.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/03/2013
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
#ifndef ESTIMATES_LOGNORMAL_H_
#define ESTIMATES_LOGNORMAL_H_

// Headers
#include "../../Estimates/Estimate.h"

// Namespaces
namespace niwa {
namespace estimates {

/**
 * Class definition
 */
class Lognormal : public niwa::Estimate {
public:
  // Methods
  Lognormal() = delete;
  explicit Lognormal(shared_ptr<Model> model);
  virtual                     ~Lognormal() = default;
  void                        DoValidate() override final { };
  Double                      GetScore() override final;

private:
  Double                      mu_ = 0;
  Double                      cv_ = 0;
  Double                      sigma_ = 0;
};

} /* namespace estimates */
} /* namespace niwa */
#endif /* PRIORS_LOGNORMAL_H_ */
