/**
 * @file ProcessProportionsAtAgeForFishery.h
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
#ifndef OBSERVATIONS_PROCESSPROPORTIONSATAGEFORFISHERY_H_
#define OBSERVATIONS_PROCESSPROPORTIONSATAGEFORFISHERY_H_

// headers
#include "Observations/Children/ProportionsAtAgeForFishery/ProportionsAtAgeForFishery.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
class ProcessProportionsAtAgeForFishery : public observations::ProportionsAtAgeForFishery {
public:
  ProcessProportionsAtAgeForFishery();
  virtual                     ~ProcessProportionsAtAgeForFishery() = default;
  void                        DoBuild() override final;

private:
  // members
  string                      process_label_;
  Double                      process_proportion_;
};

} /* namespace observations */
} /* namespace niwa */

#endif /* OBSERVATIONS_PROCESSPROPORTIONSATAGEFORFISHERY_H_ */
