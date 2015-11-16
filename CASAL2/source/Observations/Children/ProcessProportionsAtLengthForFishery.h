/**
 * @file ProcessProportionsAtLengthForFishery.h
 * @author C Marsh
 * @github https://github.com/Zaita
 * @date 25/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef OBSERVATIONS_PROCESSPROPORTIONSATLENGTHFORFISHERY_H_
#define OBSERVATIONS_PROCESSPROPORTIONSATLENGTHFORFISHERY_H_

// headers
#include "Observations/Children/ProportionsAtLengthForFishery/ProportionsAtLengthForFishery.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
class ProcessProportionsAtLengthForFishery : public observations::ProportionsAtLengthForFishery {
public:
  ProcessProportionsAtLengthForFishery(Model* model);
  virtual                     ~ProcessProportionsAtLengthForFishery() = default;
  void                        DoBuild() override final;

private:
  // members
  string                      process_label_;
  Double                      process_proportion_;
};

} /* namespace observations */
} /* namespace niwa */

#endif /* OBSERVATIONS_PROCESSPROPORTIONSATLENGTHFORFISHERY_H_ */
