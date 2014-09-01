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
namespace isam {
namespace initialisationphases {

class Derived : public isam::InitialisationPhase {
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
} /* namespace isam */

#endif /* INITIALISATIONPHASES_DERIVED_H_ */
