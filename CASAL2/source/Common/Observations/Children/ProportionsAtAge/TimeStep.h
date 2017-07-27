/**
 * @file TimeStepProportionsAtAge.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef OBSERVATIONS_TIMESTEPPROPORTIONSATAGE_H_
#define OBSERVATIONS_TIMESTEPPROPORTIONSATAGE_H_

// headers
#include "Common/Observations/Children/ProportionsAtAge.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
class TimeStepProportionsAtAge : public observations::ProportionsAtAge {
public:
  // methods
  TimeStepProportionsAtAge(Model* model);
  virtual                     ~TimeStepProportionsAtAge() = default;
  void                        DoBuild() override final;

private:
  Double                      time_step_proportion_;
};

} /* namespace observations */
} /* namespace niwa */

#endif /* TIMESTEPPROPORTIONSATAGE_H_ */
