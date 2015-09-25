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
namespace niwa {
namespace selectivities {

/**
 * Class Definition
 */
class DoubleExponential : public niwa::Selectivity {
public:
  // Methods
  DoubleExponential();
  explicit DoubleExponential(Model* model);
  virtual                     ~DoubleExponential() = default;
  void                        DoValidate() override final;
  void                        Reset() override final;

private:
  // Members
  Double                      x0_;
  Double                      x1_;
  Double                      x2_;
  Double                      y0_;
  Double                      y1_;
  Double                      y2_;
  Double                      alpha_;
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* DOUBLEEXPONENTIAL_H_ */
