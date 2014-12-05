/**
 * @file Derived.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef INITIALISATIONPHASES_DERIVED_H_
#define INITIALISATIONPHASES_DERIVED_H_

// headers
#include "InitialisationPhases/InitialisationPhase.h"

// namespaces
namespace niwa {
namespace initialisationphases {

class Derived : public niwa::InitialisationPhase {
public:
  Derived();
  virtual                     ~Derived() = default;
  void                        Execute() override final { };

protected:
  // methods
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
};

} /* namespace initialisationphases */
} /* namespace niwa */

#endif /* INITIALISATIONPHASES_DERIVED_H_ */
