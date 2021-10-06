/**
 * @file Schnute.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 12/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * TODO: Add documentation
 */
#ifndef AGELENGTHS_SCHNUTE_H_
#define AGELENGTHS_SCHNUTE_H_

// headers
#include "../../AgeLengths/AgeLength.h"

// namespaces
namespace niwa {

namespace agelengths {

// classes
class Schnute : public niwa::AgeLength {
public:
  // methods
  explicit Schnute(shared_ptr<Model> model);
  virtual ~Schnute() = default;
  void DoValidate() override final{};
  void DoBuild() override final;
  void DoReset() override final;
  // accessors

  Double calculate_mean_length(unsigned year, unsigned time_step, unsigned age) override final;

protected:
  // methods

  // members
  Double                               y1_;
  Double                               y2_;
  Double                               tau1_;
  Double                               tau2_;
  Double                               a_;
  Double                               b_;
};

} /* namespace agelengths */
} /* namespace niwa */
#endif /* AGELENGTHS_SCHNUTE_H_ */
