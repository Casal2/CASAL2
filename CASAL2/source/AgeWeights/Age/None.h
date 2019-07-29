/**
 * @file None.h
 * @author  C.Marsh
 * @date 16/11/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * TODO: Add documentation
 */
#ifndef AGEWEIGHTS_NONE_H_
#define AGEWEIGHTS_NONE_H_

// headers
#include "AgeWeights/AgeWeight.h"

// namespaces
namespace niwa {
namespace ageweights {

/**
 * class definition
 */
class None : public niwa::AgeWeight {
public:
  // methods
  explicit None(Model* model) : AgeWeight(model) { };
  virtual                     ~None() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoReset() override final { };
  void                        DoRebuildCache() override final { };

  // accessors
  Double                      mean_weight_at_age_by_year(unsigned year, unsigned age) override final { return 1.0; }

};


} /* namespace ageweights */
} /* namespace niwa */
#endif /* AGEWEIGHTS_NONE_H_ */
