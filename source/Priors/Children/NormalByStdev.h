/**
 * @file NormalByStdev.h
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
#ifndef PRIORS_NORMALBYSTDEV_H_
#define PRIORS_NORMALBYSTDEV_H_

// Headers
#include "Priors/Prior.h"

// Namespaces
namespace isam {
namespace priors {

/**
 * Class definition
 */
class NormalByStdev : public isam::Prior {
public:
  // Methods
  NormalByStdev();
  virtual                     ~NormalByStdev() noexcept(true);
  void                        Validate() override final;
  Double                      GetScore(Double param) override final;

private:
  // Members
  Double                      mu_;
  Double                      sigma_;
};

} /* namespace priors */
} /* namespace isam */
#endif /* PRIORS_NORMALBYSTDEV_H_ */
