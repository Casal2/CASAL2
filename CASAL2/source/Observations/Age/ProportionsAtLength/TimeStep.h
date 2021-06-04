/**
 * @file TimeStepProportionsAtLength.h
 * @author C Marsh
 * @github https://github.com/Zaita
 * @date 14/8/15
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef OBSERVATIONS_TIMESTEPPROPORTIONSATLENGTH_H_
#define OBSERVATIONS_TIMESTEPPROPORTIONSATLENGTH_H_

// headers
#include "Observations/Age/ProportionsAtLength.h"

// namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 *
 */
class TimeStepProportionsAtLength : public observations::age::ProportionsAtLength {
public:
  // methods
  TimeStepProportionsAtLength(shared_ptr<Model> model);
  virtual ~TimeStepProportionsAtLength() = default;
  void DoBuild() override final;

private:
  Double time_step_proportion_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* TIMESTEPPROPORTIONSATLENGTH_H_ */
