/**
 * @file ProcessAbundance.h
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
#ifndef AGE_OBSERVATIONS_PROCESSABUNDANCE_H_
#define AGE_OBSERVATIONS_PROCESSABUNDANCE_H_

// headers
#include "Observations/Age/Abundance.h"

// namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 *
 */
class ProcessAbundance : public observations::age::Abundance {
public:
  // methods
  explicit ProcessAbundance(Model* model);
  virtual                     ~ProcessAbundance() = default;
  void                        DoBuild() override final;

private:
  // members
  string                      process_label_;
  Double                      process_proportion_;
};

} /* namespace age */
} /* namespace niwa */
} /* namespace observations */

#endif /* AGE_OBSERVATIONS_PROCESSABUNDANCE_H_ */
