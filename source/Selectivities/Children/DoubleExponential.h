/**
 * @file DoubleExponential.h
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
#ifndef DOUBLEEXPONENTIAL_H_
#define DOUBLEEXPONENTIAL_H_

// Headers
#include "Selectivities/Selectivity.h"

// Namespaces
namespace isam {
namespace selectivities {

/**
 * Class Definition
 */
class DoubleExponential : public isam::Selectivity {
public:
  // Methods
  DoubleExponential();
  virtual                     ~DoubleExponential() {};
  void                        Validate() override final;
  void                        Reset() override final;

private:
  // Members
  Double                      x0_ = 0.0;
  Double                      x1_ = 0.0;
  Double                      x2_ = 0.0;
  Double                      y0_ = 0.0;
  Double                      y1_ = 0.0;
  Double                      y2_ = 0.0;
  Double                      alpha_ = 1.0;
};

} /* namespace selectivities */
} /* namespace isam */
#endif /* DOUBLEEXPONENTIAL_H_ */
