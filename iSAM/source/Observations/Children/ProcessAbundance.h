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
#ifndef OBSERVATIONS_PROCESSABUNDANCE_H_
#define OBSERVATIONS_PROCESSABUNDANCE_H_

// headers
#include "Observations/Children/Abundance/Abundance.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
class ProcessAbundance : public observations::Abundance {
public:
  // methods
  ProcessAbundance();
  virtual                     ~ProcessAbundance() = default;
  void                        DoBuild() override final;

private:
  // members
  string                      process_label_;
  Double                      process_proportion_;
};

} /* namespace observations */
} /* namespace niwa */

#endif /* OBSERVATIONS_PROCESSABUNDANCE_H_ */
