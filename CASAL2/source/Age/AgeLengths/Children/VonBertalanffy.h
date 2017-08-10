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
 * Describes an asymptotic relationship between age and length.
 */
#ifndef AGELENGTHS_VONBERTALANFFY_H_
#define AGELENGTHS_VONBERTALANFFY_H_

// headers
#include "Age/AgeLengths/AgeLength.h"

// namespaces
namespace niwa {
class LengthWeight;

namespace agelengths {

// classes
class VonBertalanffy : public niwa::AgeLength {
  friend class MockVonBertalanffy;
public:
  // methods
  explicit VonBertalanffy(Model* model);
  virtual                     ~VonBertalanffy() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        DoReset() override final;

  // accessors
  Double                      mean_length(unsigned time_step, unsigned age) override final;
  Double                      mean_weight(unsigned time_step, unsigned age) override final;

protected:
  //methods

  // members
  Double                      linf_;
  Double                      k_;
  Double                      t0_;
  string                      length_weight_label_;
  LengthWeight*               length_weight_ = nullptr;
  map<unsigned, map<unsigned, Double>> mean_length_;

};

} /* namespace agelengths */
} /* namespace niwa */
#endif /* AGELENGTHS_VONBERTALANFFY_H_ */
