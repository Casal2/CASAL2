/*
 * Manager.h
 *
 *  Created on: 13/12/2012
 *      Author: Admin
 */

#ifndef INITIALISATIONPHASES_MANAGER_H_
#define INITIALISATIONPHASES_MANAGER_H_

// Headers
#include "../BaseClasses/Manager.h"
#include "../InitialisationPhases/InitialisationPhase.h"
#include "../Model/Managers.h"

// Namespaces
namespace niwa {
class Model;

namespace initialisationphases {

/**
 * Class Definition
 */
class Manager : public base::Manager<niwa::initialisationphases::Manager, niwa::InitialisationPhase> {
  friend class base::Manager<niwa::initialisationphases::Manager, niwa::InitialisationPhase>;
  friend class niwa::Managers;
public:
  // methods
  virtual                     ~Manager() noexcept(true);
  void                        Validate() override final;
  void                        Build() override final;
  void                        Build(shared_ptr<Model> model);
  void                        Execute();
  InitialisationPhase*        GetInitPhase(const string& label);
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
  unsigned                      current_initialisation_phase_ = 0;
  unsigned                      last_executed_phase_ = 0;
  vector<InitialisationPhase*>  ordered_initialisation_phases_;

};

} /* namespace initialisationphases */
} /* namespace niwa */
#endif /* INITIALISATIONPHASES_MANAGER_H_ */
