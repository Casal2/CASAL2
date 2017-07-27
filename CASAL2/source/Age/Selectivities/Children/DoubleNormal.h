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
#include "Common/Selectivities/Selectivity.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Class definition
 */
class DoubleNormal : public niwa::Selectivity {
public:
  // Methods
  explicit DoubleNormal(Model* model);
  virtual                     ~DoubleNormal() = default;
  void                        DoValidate() override final;
  void                        Reset() override final;

protected:
  //Methods
  Double                      GetLengthBasedResult(unsigned age, AgeLength* age_length) override final;

private:
  // Members
  Double                      mu_;
  Double                      sigma_l_;
  Double                      sigma_r_;
  Double                      alpha_;
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* DOUBLENORMAL_H_ */
