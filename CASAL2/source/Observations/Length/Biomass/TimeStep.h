/**
 * @file TimeStepBiomass.h
 * @author C.Marsh
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
#ifndef LENGTH_OBSERVATIONS_TIMESTEPBIOMASS_H_
#define LENGTH_OBSERVATIONS_TIMESTEPBIOMASS_H_

// headers
#include "Observations/Length/Biomass.h"

// namespaces
namespace niwa {
namespace observations {
namespace length {

/**
 *
 */
class TimeStepBiomass : public observations::length::Biomass {
public:
  // methods
  TimeStepBiomass(shared_ptr<Model> model);
  virtual ~TimeStepBiomass() = default;
  void DoBuild() override final;

private:
  // members
  Double time_step_proportion_;
};

} /* namespace length */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_TIMESTEPBIOMASS_H_ */
