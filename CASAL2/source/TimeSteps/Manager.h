/*
 * Manager.h
 *
 *  Created on: 13/12/2012
 *      Author: Admin
 */

#ifndef TIMESTEPS_MANAGER_H_
#define TIMESTEPS_MANAGER_H_

// Headers
#include "BaseClasses/Manager.h"
#include "TimeSteps/TimeStep.h"

// Namespaces
namespace niwa {
namespace timesteps {

class DerivedQuantity;

/**
 * Class Definition
 */
class Manager : public niwa::base::Manager<niwa::timesteps::Manager, niwa::TimeStep> {
  friend class niwa::base::Manager<niwa::timesteps::Manager, niwa::TimeStep>;
  friend class niwa::Managers;
public:
  // Methods
  virtual                     ~Manager() noexcept(true);
  void                        Validate() override final;
  void                        Validate(Model* model);
  void                        Build() override final;
  void                        Execute(unsigned year);
  void                        ExecuteInitialisation(const string& phase_label, unsigned years);
  TimeStep*                   GetTimeStep(const string& label) const;
  void                        Clear() override final { objects_.clear(); ordered_time_steps_.clear(); derived_quantities_.clear(); }
  unsigned                    GetTimeStepIndex(const string& time_step_label) const;
  unsigned                    GetTimeStepIndexForProcess(const string& process_label) const;
  unsigned                    GetTimeStepCountForProcess(const string& process_label) const;
  vector<unsigned>            GetTimeStepIndexesForProcess(const string& process_label) const;
  vector<ProcessType>         GetOrderedProcessTypes();

  // accessors
  const vector<TimeStep*>&    ordered_time_steps() const { return ordered_time_steps_; }
  virtual unsigned            current_time_step() const { return current_time_step_; }

protected:
  // methods
  Manager();

  // Members
  Model*                      model_;
  vector<TimeStep*>           ordered_time_steps_;
  vector<DerivedQuantity*>    derived_quantities_;
  unsigned                    current_time_step_ = 0;
};

} /* namespace timesteps */
} /* namespace niwa */
#endif /* TIMESTEPS_MANAGER_H_ */
