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
#ifndef AGELENGTHS_VONBERTALANFFY_H_
#define AGELENGTHS_VONBERTALANFFY_H_

// headers
#include "AgeLengths/AgeLength.h"
#include "LengthWeights/LengthWeight.h"

// namespaces
namespace niwa {
namespace agelengths {

/**
 * class definition
 */
class VonBertalanffy : public niwa::AgeLength {
public:
  // methods
  VonBertalanffy();
  virtual                     ~VonBertalanffy() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        DoReset() override final { };

  // accessors
  Double                      mean_length(unsigned year, unsigned age) override final;
  Double                      mean_weight(unsigned year, unsigned age) override final;

private:
  // members
  Double                      linf_;
  Double                      k_;
  Double                      t0_;
  Double                      cv_;
  string                      distribution_;
  bool                        by_length_;
  string                      length_weight_label_;
  LengthWeightPtr             length_weight_;
};

} /* namespace agelengths */
} /* namespace niwa */
#endif /* AGELENGTHS_VONBERTALANFFY_H_ */
