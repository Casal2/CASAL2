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
#include "Age/Processes/Children/RecruitmentBevertonHolt.h"
#include "Age/Processes/Children/RecruitmentBevertonHoltWithDeviations.h"
#include "Common/InitialisationPhases/InitialisationPhase.h"
#include "Common/Partition/Accessors/Cached/Categories.h"
#include "Common/Partition/Accessors/Categories.h"
// TODO: Move this to age

// namespaces
namespace niwa {
class TimeStep;

namespace initialisationphases {
namespace cached   = partition::accessors::cached;
namespace accessor = partition::accessors;
using age::processes::RecruitmentBevertonHolt;
using age::processes::RecruitmentBevertonHoltWithDeviations;

// classes
class Derived : public niwa::InitialisationPhase {
public:
  // methods
  explicit Derived(Model* model);
  virtual                     ~Derived() = default;
  void                        Execute() override final;

protected:
  // methods
  void                        DoValidate() override final;
  void                        DoBuild() override final;

  // members
  unsigned                    years_;
  vector<string>              insert_processes_;
  bool                        recruitment_ = false;
  vector<string>              exclude_processes_;
  vector<TimeStep*>           time_steps_;
  cached::Categories          cached_partition_;
  accessor::Categories        partition_;
  vector<RecruitmentBevertonHolt*> recruitment_process_;
  vector<RecruitmentBevertonHoltWithDeviations*> recruitment_process_with_devs_;
  bool                        casal_initialisation_phase_ = false;
};

} /* namespace initialisationphases */
} /* namespace niwa */
#endif /* INITIALISATIONPHASES_DERIVED_H_ */
