/*
 * Manager.h
 *
 *  Created on: 13/12/2012
 *      Author: Admin
 */

#ifndef INITIALISATIONPHASES_MANAGER_H_
#define INITIALISATIONPHASES_MANAGER_H_

// Headers
#include "BaseClasses/Manager.h"
#include "InitialisationPhases/InitialisationPhase.h"

// Namespaces
namespace isam {
namespace initialisationphases {

/**
 * Class Definition
 */
class Manager : public base::Manager<isam::initialisationphases::Manager, isam::InitialisationPhase> {
public:
  // Methods
  Manager();
  virtual                     ~Manager() noexcept(true);
  void                        Build() override final;
  void                        Execute();
  bool                        IsPhaseDefined(const string& label);

private:
  // Members
  vector<InitialisationPhasePtr>  ordered_initialisation_phases_;
};

} /* namespace initialisationphases */
} /* namespace isam */
#endif /* INITIALISATIONPHASES_MANAGER_H_ */
