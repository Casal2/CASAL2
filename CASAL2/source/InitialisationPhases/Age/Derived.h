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
#include "Processes/Age/RecruitmentBevertonHolt.h"
#include "Processes/Age/RecruitmentBevertonHoltWithDeviations.h"
#include "InitialisationPhases/InitialisationPhase.h"
#include "Partition/Accessors/Cached/Categories.h"
#include "Partition/Accessors/Categories.h"

// namespaces
namespace niwa {
class TimeStep;
namespace initialisationphases {
namespace age {
namespace cached   = partition::accessors::cached;
namespace accessor = partition::accessors;
using processes::age::RecruitmentBevertonHolt;
using processes::age::RecruitmentBevertonHoltWithDeviations;

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

} /* namespace age */
} /* namespace initialisationphases */
} /* namespace niwa */
#endif /* INITIALISATIONPHASES_DERIVED_H_ */
