/**
 * @file ProcessBiomass.h
 * @author C.Marsh
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
#ifndef LENGTH_OBSERVATIONS_PROCESSBIOMASS_H_
#define LENGTH_OBSERVATIONS_PROCESSBIOMASS_H_

// headers
#include "../Biomass.h"

// namespaces
namespace niwa {
namespace observations {
namespace length {

/**
 *
 */
class ProcessBiomass : public observations::length::Biomass {
public:
  explicit ProcessBiomass(shared_ptr<Model> model);
  virtual ~ProcessBiomass() = default;
  void DoBuild() override final;

private:
  // members
  string process_label_;
  Double process_proportion_;
};

} /* namespace length */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_PROCESSBIOMASS_H_ */
