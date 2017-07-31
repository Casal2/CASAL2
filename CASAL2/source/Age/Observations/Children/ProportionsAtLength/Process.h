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
#ifndef AGE_OBSERVATIONS_PROCESSPROPORTIONSATLENGTH_H_
#define AGE_OBSERVATIONS_PROCESSPROPORTIONSATLENGTH_H_

// headers
#include "Age/Observations/Children/ProportionsAtLength.h"

// namespaces
namespace niwa {
namespace age {
namespace observations {

/**
 *
 */
class ProcessProportionsAtLength : public observations::ProportionsAtLength {
public:
  ProcessProportionsAtLength(Model* model);
  virtual                     ~ProcessProportionsAtLength() = default;
  void                        DoBuild() override final;

private:
  // members
  string                      process_label_;
  Double                      process_proportion_;
};

} /* namespace observations */
} /* namespace age */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_PROCESSPROPORTIONSATLENGTH_H_ */
