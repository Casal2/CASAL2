/**
 * @file Uniform.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This prior will always return 0, regardless of input or parameters
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PRIORS_UNIFORM_H_
#define PRIORS_UNIFORM_H_

// Headers
#include "Priors/Prior.h"

// namespaces
namespace isam {
namespace priors {

/**
 * Class definition
 */
class Uniform : public isam::Prior {
public:
  // Methods
  Uniform();
  virtual                     ~Uniform() = default;
  void                        DoValidate() override final { };
  Double                      GetScore(Double param) override final { return 0; }
};

} /* namespace priors */
} /* namespace isam */
#endif /* PRIORS_UNIFORM_H_ */
