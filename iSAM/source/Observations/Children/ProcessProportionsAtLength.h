/**
 * @file ProcessProportionsAtLength.h
 * @author C Marsh
 * @github https://github.com/Zaita
 * @date 14/08/15
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef OBSERVATIONS_PROCESSPROPORTIONSATLENGTH_H_
#define OBSERVATIONS_PROCESSPROPORTIONSATLENGTH_H_

// headers
#include "Observations/Children/ProportionsAtLength/ProportionsAtLength.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
class ProcessProportionsAtLength : public observations::ProportionsAtLength {
public:
  ProcessProportionsAtLength();
  virtual                     ~ProcessProportionsAtLength() = default;
  void                        DoBuild() override final;

private:
  // members
  string                      process_label_;
  Double                      process_proportion_;
};

} /* namespace observations */
} /* namespace niwa */

#endif /* PROCESSPROPORTIONSATLENGTH_H_ */
