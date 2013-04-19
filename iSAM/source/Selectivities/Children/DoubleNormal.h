/**
 * @file DoubleNormal.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 14/01/2013
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
#ifndef DOUBLENORMAL_H_
#define DOUBLENORMAL_H_

// Headers
#include "Selectivities/Selectivity.h"

// Namespaces
namespace isam {
namespace selectivities {

/**
 * Class definition
 */
class DoubleNormal : public isam::Selectivity {
public:
  // Methods
  DoubleNormal();
  explicit DoubleNormal(ModelPtr model);
  virtual                     ~DoubleNormal() {};
  void                        Validate() override final;
  void                        Reset() override final;

private:
  // Members
  Double                      mu_ = 0.0;
  Double                      sigma_l_ = 0.0;
  Double                      sigma_r_ = 0.0;
  Double                      alpha_ = 1.0;
};

} /* namespace selectivities */
} /* namespace isam */
#endif /* DOUBLENORMAL_H_ */
