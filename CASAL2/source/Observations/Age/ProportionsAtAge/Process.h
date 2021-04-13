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
#ifndef AGE_OBSERVATIONS_PROCESSPROPORTIONSATAGE_H_
#define AGE_OBSERVATIONS_PROCESSPROPORTIONSATAGE_H_

// headers
#include "Observations/Age/ProportionsAtAge.h"

// namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 *
 */
class ProcessProportionsAtAge : public observations::age::ProportionsAtAge {
public:
  ProcessProportionsAtAge(shared_ptr<Model> model);
  virtual                     ~ProcessProportionsAtAge() = default;
  void                        DoBuild() override final;

private:
  // members
  string                      process_label_;
  Double                      process_proportion_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_PROCESSPROPORTIONSATAGE_H_ */
