/**
 * @file TimeStepAbundance.h
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef LENGTH_OBSERVASTIONS_TIMESTEPABUNDANCE_H_
#define LENGTH_OBSERVASTIONS_TIMESTEPABUNDANCE_H_

// headers
#include "../Abundance.h"

// namespaces
namespace niwa {
namespace observations {
namespace length {

/**
 *
 */
class TimeStepAbundance : public observations::length::Abundance {
public:
  // methods
  TimeStepAbundance(shared_ptr<Model> model);
  virtual ~TimeStepAbundance() = default;
  void DoBuild() override final;

private:
  Double time_step_proportion_;
};

} /* namespace length */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVASTIONS_TIMESTEPABUNDANCE_H_ */
