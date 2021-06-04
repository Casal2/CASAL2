/**
 * @file TimeStepProportionsAtAge.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGE_OBSERVATIONS_TIMESTEPPROPORTIONSATAGE_H_
#define AGE_OBSERVATIONS_TIMESTEPPROPORTIONSATAGE_H_

// headers
#include "Observations/Age/ProportionsAtAge.h"

// namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 *
 */
class TimeStepProportionsAtAge : public observations::age::ProportionsAtAge {
public:
  // methods
  TimeStepProportionsAtAge(shared_ptr<Model> model);
  virtual ~TimeStepProportionsAtAge() = default;
  void DoBuild() override final;

private:
  Double time_step_proportion_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_TIMESTEPPROPORTIONSATAGE_H_ */
