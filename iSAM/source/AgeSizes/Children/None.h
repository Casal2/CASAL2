/**
 * @file None.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGESIZES_NONE_H_
#define AGESIZES_NONE_H_

// headers
#include "AgeSizes/AgeSize.h"

// namespaces
namespace niwa {
namespace agesizes {

/**
 * class definition
 */
class None : public niwa::AgeSize {
public:
  // methods
  None();
  virtual                     ~None() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoReset() override final { };

  // accessors
  Double                      mean_size(unsigned year, unsigned age) override final { return 1.0; }
  Double                      mean_weight(unsigned year, unsigned age) override final { return 1.0; }
};

} /* namespace agesizes */
} /* namespace niwa */
#endif /* AGESIZES_NONE_H_ */
