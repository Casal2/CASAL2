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
#include "Partition/Accessors/Categories.h"
#include "Partition/Accessors/Cached/Categories.h"

#include "Processes/Children/RecruitmentBevertonHolt.h"
#include "Processes/Children/RecruitmentBevertonHoltWithDeviations.h"
// namespaces
namespace niwa {
class TimeStep;

namespace initialisationphases {
namespace cached   = partition::accessors::cached;
namespace accessor = partition::accessors;
using processes::RecruitmentBevertonHolt;
using processes::RecruitmentBevertonHoltWithDeviations;
/**
 *
 */
class Iterative : public niwa::InitialisationPhase {
public:
  // methods
  explicit Iterative(Model* model);
  virtual                     ~Iterative() = default;
  void                        Execute() override final;

protected:
  // methods
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  bool                        CheckConvergence();

  // members
  unsigned                    years_;
  vector<string>              insert_processes_;
  vector<string>              exclude_processes_;
  vector<TimeStep*>           time_steps_;
  Double                      lambda_;
  vector<unsigned>            convergence_years_;
  cached::Categories          cached_partition_;
  accessor::Categories        partition_;

  vector<RecruitmentBevertonHolt*> recruitment_process_;
  vector<RecruitmentBevertonHoltWithDeviations*> recruitment_process_with_devs_;
};

} /* namespace base */
} /* namespace niwa */

#endif /* ITERATIVE_H_ */
