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
#include "Age/AgeLengths/AgeLength.h"

// namespaces
namespace niwa {
class LengthWeight;

namespace agelengths {

// classes
class Schnute : public niwa::AgeLength {
public:
  // methods
  explicit Schnute(Model* model);
  virtual                     ~Schnute() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        DoReset() override final { };

  // accessors
  Double                      mean_length(unsigned time_step, unsigned age) override final;
  Double                      mean_weight(unsigned time_step, unsigned age) override final;

protected:
  // methods

  // members
  Double                      y1_;
  Double                      y2_;
  Double                      tau1_;
  Double                      tau2_;
  Double                      a_;
  Double                      b_;
  string                      length_weight_label_;
  LengthWeight*               length_weight_ = nullptr;
};

} /* namespace agelengths */
} /* namespace niwa */
#endif /* AGELENGTHS_SCHNUTE_H_ */
