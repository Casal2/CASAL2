/**
 * @file ProcessProportionsMigrating.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 8/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef OBSERVATIONS_PROCESSPROPORTIONSMIGRATING_H_
#define OBSERVATIONS_PROCESSPROPORTIONSMIGRATING_H_

// headers
#include "Observations/Children/ProportionsMigrating/ProportionsMigrating.h"

// namespaces
namespace niwa {
namespace observations {

/**
 *
 */
class ProcessProportionsMigrating : public observations::ProportionsMigrating {
public:
  explicit ProcessProportionsMigrating(Model* model);
  virtual                     ~ProcessProportionsMigrating() = default;
  void                        DoBuild() override final;

private:
  // members
  string                      process_label_;
  Double                      process_proportion_;
};

} /* namespace observations */
} /* namespace niwa */

#endif /* OBSERVATIONS_PROCESSPROPORTIONSMIGRATING_H_ */
