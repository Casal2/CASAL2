/**
 * @file ProcessProportionsAtLength.h
 * @author C Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef LENGTH_OBSERVATIONS_PROCESSPROPORTIONSATLENGTH_H_
#define LENGTH_OBSERVATIONS_PROCESSPROPORTIONSATLENGTH_H_

// headers
#include "Observations/Length/ProportionsAtLength.h"

// namespaces
namespace niwa {
namespace observations {
namespace length {

/**
 *
 */
class ProcessProportionsAtLength : public observations::length::ProportionsAtLength {
public:
  ProcessProportionsAtLength(shared_ptr<Model> model);
  virtual ~ProcessProportionsAtLength() = default;
  void DoBuild() override final;

private:
  // members
  string process_label_;
  Double process_proportion_;
};

} /* namespace length */
} /* namespace observations */
} /* namespace niwa */

#endif /* LENGTH_OBSERVATIONS_PROCESSPROPORTIONSATLENGTH_H_ */
