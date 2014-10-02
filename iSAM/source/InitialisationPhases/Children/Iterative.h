/**
 * @file Iterative.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This type of initialisation phases is our de-facto one that
 * does a year/time-step iterative approach.
 */
#ifndef INITIALISATIONPHASES_ITERATIVE_H_
#define INITIALISATIONPHASES_ITERATIVE_H_

// headers
#include "InitialisationPhases/InitialisationPhase.h"

// namespaces
namespace isam {
namespace initialisationphases {

/**
 *
 */
class Iterative : public isam::InitialisationPhase {
public:
  // methods
  Iterative();
  virtual                     ~Iterative() = default;
  void                        Execute() override final;

protected:
  // methods
  void                        DoValidate() override final;
  void                        DoBuild() override final;

  // members
  unsigned                    years_;
  vector<string>              insert_processes_;
  vector<string>              exclude_processes_;
  vector<TimeStepPtr>         time_steps_;
};

} /* namespace base */
} /* namespace isam */

#endif /* ITERATIVE_H_ */
