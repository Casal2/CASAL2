/**
 * @file ProcessProportionsAtAge.h
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
#ifndef OBSERVATIONS_PROCESSPROPORTIONSATAGE_H_
#define OBSERVATIONS_PROCESSPROPORTIONSATAGE_H_

// headers
#include "Observations/Children/ProportionsAtAge/ProportionsAtAge.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
class ProcessProportionsAtAge : public observations::ProportionsAtAge {
public:
  ProcessProportionsAtAge(Model* model);
  virtual                     ~ProcessProportionsAtAge() = default;
  void                        DoBuild() override final;

private:
  // members
  string                      process_label_;
  Double                      process_proportion_;
};

} /* namespace observations */
} /* namespace niwa */

#endif /* PROCESSPROPORTIONSATAGE_H_ */
