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
#ifndef SIZEWEIGHTS_NONE_H_
#define SIZEWEIGHTS_NONE_H_

// headers
#include "SizeWeights/SizeWeight.h"

// namespaces
namespace isam {
namespace sizeweights {

/**
 * class definition
 */
class None : public isam::SizeWeight {
public:
  // methods
  None();
  virtual                     ~None() = default;
  void                        DoValidate() { };
  void                        DoBuild() { };
  void                        DoReset() { };

  // accessors
  Double                      mean_weight(Double size) const override final { return 1.0; }
};

} /* namespace sizeweights */
} /* namespace isam */
#endif /* SIZEWEIGHTS_NONE_H_ */
