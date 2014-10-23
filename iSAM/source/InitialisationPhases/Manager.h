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
class Manager : public oldbase::Manager<isam::initialisationphases::Manager, isam::InitialisationPhase> {
  friend class oldbase::Manager<isam::initialisationphases::Manager, isam::InitialisationPhase>;
public:
  // methods
  virtual                     ~Manager() noexcept(true);
  void                        Build() override final;
  void                        Execute();
  bool                        IsPhaseDefined(const string& label) const;
  unsigned                    GetPhaseIndex(const string& label) const;

  // accessors
  unsigned                    last_executed_phase() const { return last_executed_phase_; }
  unsigned                    current_initialisation_phase() const { return current_initialisation_phase_; }

protected:
  // methods
  Manager();

private:
  // members
  unsigned                        current_initialisation_phase_;
  unsigned                        last_executed_phase_ = 0;
  vector<InitialisationPhasePtr>  ordered_initialisation_phases_;

};

} /* namespace initialisationphases */
} /* namespace isam */
#endif /* INITIALISATIONPHASES_MANAGER_H_ */
