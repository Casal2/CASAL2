/**
 * @file TimeStepTagRecaptureByLength.h
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 29/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Tag recapture by age Observation in a timestep
 */
#ifndef OBSERVATIONS_TIMESTEPTAGRECAPTUREBYLENGTH_H_
#define OBSERVATIONS_TIMESTEPTAGRECAPTUREBYLENGTH_H_

// headers
#include <Observations/Children/TagRecaptureByLength/TagRecaptureByLength.h>

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
class TimeStepTagRecaptureByLength : public observations::TagRecaptureByLength {
public:
  // methods
  TimeStepTagRecaptureByLength(Model* model);
  virtual                     ~TimeStepTagRecaptureByLength() = default;
  void                        DoBuild() override final;

private:
  Double                      time_step_proportion_;
};

} /* namespace observations */
} /* namespace niwa */

#endif /* OBSERVATIONS_TIMESTEPTAGRECAPTUREBYLENGTH_H_ */
