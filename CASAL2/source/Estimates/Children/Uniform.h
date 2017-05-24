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
#ifndef ESTIMATES_UNIFORM_H_
#define ESTIMATES_UNIFORM_H_

// Headers
#include "Estimates/Estimate.h"

// namespaces
namespace niwa {
namespace estimates {

/**
 * Class definition
 */
class Uniform : public niwa::Estimate {
public:
  // Methods
  Uniform() = delete;
  explicit Uniform(Model* model);
  virtual                     ~Uniform() = default;
  void                        DoValidate() override final { };
  Double                      GetScore() override final { return 0; }
};

} /* namespace estimates */
} /* namespace niwa */
#endif /* ESTIMATES_UNIFORM_H_ */
