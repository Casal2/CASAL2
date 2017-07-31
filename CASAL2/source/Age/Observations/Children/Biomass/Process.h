/**
 * @file ProcessBiomass.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGE_OBSERVATIONS_PROCESSBIOMASS_H_
#define AGE_OBSERVATIONS_PROCESSBIOMASS_H_

// headers
#include "Age/Observations/Children/Biomass.h"

// namespaces
namespace niwa {
namespace age {
namespace observations {

/**
 *
 */
class ProcessBiomass : public observations::Biomass {
public:
  explicit ProcessBiomass(Model* model);
  virtual                     ~ProcessBiomass() = default;
  void                        DoBuild() override final;

private:
  // members
  string                      process_label_;
  Double                      process_proportion_;
};

} /* namespace observations */
} /* namespace age */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_PROCESSBIOMASS_H_ */
