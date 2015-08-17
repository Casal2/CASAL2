/**
 * @file TimeStepProportionsAtLength.h
 * @author C Marsh
 * @github https://github.com/Zaita
 * @date 14/8/15
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef OBSERVATIONS_TIMESTEPPROPORTIONSATLENGTH_H_
#define OBSERVATIONS_TIMESTEPPROPORTIONSATLENGTH_H_

// headers
#include "Observations/Children/ProportionsAtLength/ProportionsAtLength.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
class TimeStepProportionsAtLength : public observations::ProportionsAtLength {
public:
  // methods
  TimeStepProportionsAtLength();
  virtual                     ~TimeStepProportionsAtLength() = default;
  void                        DoBuild() override final;

private:
  Double                      time_step_proportion_;
};

} /* namespace observations */
} /* namespace niwa */

#endif /* TIMESTEPPROPORTIONSATLENGTH_H_ */
