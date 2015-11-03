/**
 * @file TimeStepTagRecaptureByAge.h
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 27/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Tag recapture Observation in a timestep
 */
#ifndef OBSERVATIONS_TIMESTEPTAGRECAPTUREBYAGE_H_
#define OBSERVATIONS_TIMESTEPTAGRECAPTUREBYAGE_H_

// headers
#include <Observations/Children/TagRecaptureByAge/TagRecaptureByAge.h>

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
class TimeStepTagRecaptureByAge : public observations::TagRecaptureByAge {
public:
  // methods
  TimeStepTagRecaptureByAge(Model* model);
  virtual                     ~TimeStepTagRecaptureByAge() = default;
  void                        DoBuild() override final;

private:
  Double                      time_step_proportion_;
};

} /* namespace observations */
} /* namespace niwa */

#endif /* OBSERVATIONS_TIMESTEPTAGRECAPTUREBYAGE_H_ */
