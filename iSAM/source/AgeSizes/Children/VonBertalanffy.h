/**
 * @file VonBertalanffy.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 14/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef VONBERTALANFFY_H_
#define VONBERTALANFFY_H_

// headers
#include "AgeSizes/AgeSize.h"
#include "SizeWeights/SizeWeight.h"

// namespaces
namespace isam {
namespace agesizes {

/**
 * class definition
 */
class VonBertalanffy : public isam::AgeSize {
public:
  // methods
  VonBertalanffy();
  virtual                     ~VonBertalanffy() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };

  // accessors
  Double                      mean_size(unsigned year, unsigned age) const override final;
  Double                      mean_weight(unsigned year, unsigned age) const override final;

private:
  // members
  Double                      linf_;
  Double                      k_;
  Double                      t0_;
  Double                      cv_;
  string                      distribution_;
  bool                        by_length_;
  string                      size_weight_label_;
  SizeWeightPtr               size_weight_;
};

} /* namespace agesizes */
} /* namespace isam */
#endif /* VONBERTALANFFY_H_ */
