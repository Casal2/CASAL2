/**
 * @file Schnute.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 12/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * TODO: Add documentation
 */
#ifndef AGELENGTHS_SCHNUTE_H_
#define AGELENGTHS_SCHNUTE_H_

// headers
#include "AgeLengths/AgeLength.h"
#include "LengthWeights/LengthWeight.h"

// namespaces
namespace niwa {
namespace agelengths {

// classes
class Schnute : public niwa::AgeLength {
public:
  // methods
  Schnute();
  explicit Schnute(ModelPtr model);
  virtual                     ~Schnute() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        BuildCV(unsigned year) override final;

  // accessors
  Double                      mean_length(unsigned year, unsigned age) override final;
  Double                      mean_weight(unsigned year, unsigned age) override final;

protected:
  // members
  Double                      y1_;
  Double                      y2_;
  Double                      tau1_;
  Double                      tau2_;
  Double                      a_;
  Double                      b_;
  bool                        by_length_;
  string                      length_weight_label_;
  LengthWeightPtr             length_weight_;
};

} /* namespace agelengths */
} /* namespace niwa */
#endif /* AGELENGTHS_SCHNUTE_H_ */
