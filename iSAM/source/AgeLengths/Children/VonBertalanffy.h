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
 * TODO: Add documentation
 */
#ifndef AGELENGTHS_VONBERTALANFFY_H_
#define AGELENGTHS_VONBERTALANFFY_H_

// headers
#include "AgeLengths/AgeLength.h"
#include "LengthWeights/LengthWeight.h"

// namespaces
namespace niwa {
namespace agelengths {

// classes
class VonBertalanffy : public niwa::AgeLength {
  friend class MockVonBertalanffy;
public:
  // methods
  VonBertalanffy();
  explicit VonBertalanffy(ModelPtr model);
  virtual                     ~VonBertalanffy() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        BuildCV(unsigned year) override final;
  void                        DoAgeToLengthConversion(std::shared_ptr<partition::Category> category, const vector<Double>& length_bins) override final;

  // accessors
  Double                      mean_length(unsigned year, unsigned age) override final;
  Double                      mean_weight(unsigned year, unsigned age) override final;

protected:
  // methods
  void                        CummulativeNormal(Double mu, Double cv, vector<Double>& prop_in_length, vector<Double> length_bins, string distribution, bool plus_grp);

  // members
  Double                      linf_;
  Double                      k_;
  Double                      t0_;
  string                      distribution_;
  bool                        by_length_;
  string                      length_weight_label_;
  LengthWeightPtr             length_weight_;
};

} /* namespace agelengths */
} /* namespace niwa */
#endif /* AGELENGTHS_VONBERTALANFFY_H_ */
