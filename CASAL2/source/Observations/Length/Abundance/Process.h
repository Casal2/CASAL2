/**
 * @file ProcessAbundance.h
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef LENGTH_OBSERVATIONS_PROCESSABUNDANCE_H_
#define LENGTH_OBSERVATIONS_PROCESSABUNDANCE_H_

// headers
#include "Observations/Length/Abundance.h"

// namespaces
namespace niwa {
namespace observations {
namespace length {

/**
 *
 */
class ProcessAbundance : public observations::length::Abundance {
public:
  // methods
  explicit ProcessAbundance(shared_ptr<Model> model);
  virtual ~ProcessAbundance() = default;
  void DoBuild() override final;

private:
  // members
  string process_label_;
  Double process_proportion_;
};

} /* namespace length */
}  // namespace observations
}  // namespace niwa

#endif /* AGE_OBSERVATIONS_PROCESSABUNDANCE_H_ */
