/**
 * @file TimeStepAbundance.h
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
#ifndef OBSERVASTIONS_TIMESTEPABUNDANCE_H_
#define OBSERVASTIONS_TIMESTEPABUNDANCE_H_

// headers
#include "Common/Observations/Children/Abundance.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
class TimeStepAbundance : public observations::Abundance {
public:
  // methods
  TimeStepAbundance(Model* model);
  virtual                     ~TimeStepAbundance() = default;
  void                        DoBuild() override final;

private:
  Double                      time_step_proportion_;
};

} /* namespace observations */
} /* namespace niwa */

#endif /* OBSERVASTIONS_TIMESTEPABUNDANCE_H_ */
