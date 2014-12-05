/**
 * @file Constant.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 8/08/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SIMULATES_CONSTANT_H_
#define SIMULATES_CONSTANT_H_

// headers
#include "Simulates/Simulate.h"

// namespaces
namespace niwa {
namespace simulates {

/**
 * Class definition
 */
class Constant : public niwa::Simulate {
public:
  Constant();
  virtual                     ~Constant() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoUpdate() override final;

private:
  // members
  Double                      value_;
};

} /* namespace simulates */
} /* namespace niwa */

#endif /* CONSTANT_H_ */
