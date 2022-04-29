/**
 * @file TimeStepProportionsAtLength.h
 * @author C Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef LENGTH_OBSERVATIONS_TIMESTEPPROPORTIONSATLENGTH_H_
#define LENGTH_OBSERVATIONS_TIMESTEPPROPORTIONSATLENGTH_H_

// headers
#include "Observations/Length/ProportionsAtLength.h"

// namespaces
namespace niwa {
namespace observations {
namespace length {

/**
 *
 */
class TimeStepProportionsAtLength : public observations::length::ProportionsAtLength {
public:
  // methods
  TimeStepProportionsAtLength(shared_ptr<Model> model);
  virtual ~TimeStepProportionsAtLength() = default;
  void DoBuild() override final;

private:
  Double time_step_proportion_;
};

} /* namespace length */
} /* namespace observations */
} /* namespace niwa */

#endif /* TIMESTEPPROPORTIONSATLENGTH_H_ */
