/**
 * @file ProcessProportionsAtLength.h
 * @author C Marsh
 * @github https://github.com/Zaita
 * @date 14/08/15
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGE_OBSERVATIONS_PROCESSPROPORTIONSATLENGTH_H_
#define AGE_OBSERVATIONS_PROCESSPROPORTIONSATLENGTH_H_

// headers
#include "Observations/Age/ProportionsAtLength.h"

// namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 *
 */
class ProcessProportionsAtLength : public observations::age::ProportionsAtLength {
public:
  ProcessProportionsAtLength(shared_ptr<Model> model);
  virtual ~ProcessProportionsAtLength() = default;
  void DoBuild() override final;

private:
  // members
  string process_label_;
  Double process_proportion_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_PROCESSPROPORTIONSATLENGTH_H_ */
