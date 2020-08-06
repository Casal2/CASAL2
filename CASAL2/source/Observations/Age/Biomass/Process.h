/**
 * @file ProcessBiomass.h
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
#ifndef AGE_OBSERVATIONS_PROCESSBIOMASS_H_
#define AGE_OBSERVATIONS_PROCESSBIOMASS_H_

// headers
#include "Observations/Age/Biomass.h"

// namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 *
 */
class ProcessBiomass : public observations::age::Biomass {
public:
  explicit ProcessBiomass(Model* model);
  virtual                     ~ProcessBiomass() = default;
  void                        DoBuild() override final;

private:
  // members
  string                      process_label_;
  double                      process_proportion_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_PROCESSBIOMASS_H_ */
