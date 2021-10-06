/**
 * @file VonBertalanffy.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 14/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Describes an asymptotic relationship between age and length.
 */
#ifndef AGELENGTHS_VONBERTALANFFY_H_
#define AGELENGTHS_VONBERTALANFFY_H_

// headers
#include "../../AgeLengths/AgeLength.h"

// namespaces
namespace niwa {
namespace agelengths {

// classes
class VonBertalanffy : public niwa::AgeLength {
  friend class MockVonBertalanffy;

public:
  // methods
  explicit VonBertalanffy(shared_ptr<Model> model);
  virtual ~VonBertalanffy() = default;
  void DoValidate() override final{};
  void DoBuild() override final;
  void DoReset() override final;

  // accessors  
  Double calculate_mean_length(unsigned year, unsigned time_step, unsigned age) override final;

protected:
  // methods

  // members
  Double                               linf_;
  Double                               k_;
  Double                               t0_;
};

} /* namespace agelengths */
} /* namespace niwa */
#endif /* AGELENGTHS_VONBERTALANFFY_H_ */
