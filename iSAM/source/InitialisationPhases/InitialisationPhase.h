/**
 * @file InitialisationPhase.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef INITIALISATIONPHASE_H_
#define INITIALISATIONPHASE_H_

// Headers
#include "BaseClasses/Object.h"
#include "TimeSteps/TimeStep.h"

namespace isam {

/**
 * Class Defintiion
 */
class InitialisationPhase : public isam::base::Object {
public:
  // Methods
  InitialisationPhase();
  virtual                     ~InitialisationPhase() = default;
  void                        Validate();
  void                        Build();
  void                        Reset() { };
  void                        Execute();

private:
  // Members
  unsigned                    years_;
  vector<string>              process_labels_;
  TimeStepPtr                 time_step_;
};

/*
 * Typedef
 */
typedef boost::shared_ptr<InitialisationPhase> InitialisationPhasePtr;

} /* namespace isam */
#endif /* INITIALISATIONPHASE_H_ */
