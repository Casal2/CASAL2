/**
 * @file TimeStepAbundance.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGE_OBSERVASTIONS_TIMESTEPABUNDANCE_H_
#define AGE_OBSERVASTIONS_TIMESTEPABUNDANCE_H_

// headers
#include "../Abundance.h"

// namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 *
 */
class TimeStepAbundance : public observations::age::Abundance {
public:
  // methods
  TimeStepAbundance(shared_ptr<Model> model);
  virtual ~TimeStepAbundance() = default;
  void DoBuild() override final;

private:
  Double time_step_proportion_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVASTIONS_TIMESTEPABUNDANCE_H_ */
