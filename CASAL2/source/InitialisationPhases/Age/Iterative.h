/**
 * @file Iterative.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This type of initialisation phases is our de-facto one that
 * does a year/time-step iterative approach.
 */
#ifndef INITIALISATIONPHASES_ITERATIVE_H_
#define INITIALISATIONPHASES_ITERATIVE_H_

// headers
#include "../../InitialisationPhases/InitialisationPhase.h"
#include "../../Partition/Accessors/Cached/Categories.h"
#include "../../Partition/Accessors/Categories.h"
#include "../../Processes/Age/RecruitmentBevertonHolt.h"
#include "../../Processes/Age/RecruitmentBevertonHoltWithDeviations.h"

// namespaces
namespace niwa {
class TimeStep;
namespace initialisationphases {
namespace age {
namespace cached   = partition::accessors::cached;
namespace accessor = partition::accessors;
using processes::age::RecruitmentBevertonHolt;
using processes::age::RecruitmentBevertonHoltWithDeviations;
/**
 *
 */
class Iterative : public niwa::InitialisationPhase {
public:
  // methods
  explicit Iterative(shared_ptr<Model> model);
  virtual ~Iterative() = default;

protected:
  // methods
  void DoValidate() override final;
  void DoBuild() override final;
  void DoExecute() override final;
  bool CheckConvergence();

  // members
  unsigned             years_;
  vector<string>       insert_processes_;
  vector<string>       exclude_processes_;
  vector<TimeStep*>    time_steps_;
  Double               lambda_;
  vector<unsigned>     convergence_years_;
  cached::Categories   cached_partition_;
  accessor::Categories partition_;

  vector<RecruitmentBevertonHolt*>               recruitment_process_;
  vector<RecruitmentBevertonHoltWithDeviations*> recruitment_process_with_devs_;
};

}  // namespace age
} /* namespace initialisationphases */
} /* namespace niwa */

#endif /* ITERATIVE_H_ */
