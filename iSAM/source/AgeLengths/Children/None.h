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
 * TODO: Add documentation
 */
#ifndef AGELENGTHS_NONE_H_
#define AGELENGTHS_NONE_H_

// headers
#include "AgeLengths/AgeLength.h"

// namespaces
namespace niwa {
namespace agelengths {

/**
 * class definition
 */
class None : public niwa::AgeLength {
public:
  // methods
  None();
  explicit None(ModelPtr model) : AgeLength(Model::Instance()) { };
  virtual                     ~None() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoReset() override final { };
  void                        BuildCV(unsigned year) override final { };
  void                        DoAgeToLengthConversion(partition::Category* category, const vector<Double>& length_bins, bool plus_grp, SelectivityPtr selectivity) override final { };

  // accessors
  Double                      mean_length(unsigned year, unsigned age) override final { return 1.0; }
  Double                      mean_weight(unsigned year, unsigned age) override final { return 1.0; }
};


} /* namespace agelengths */
} /* namespace niwa */
#endif /* AGELENGTHS_NONE_H_ */
