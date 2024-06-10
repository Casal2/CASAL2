/**
 * @file Iterative.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2/09/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This type of initialisation phases is our de-facto one that
 * does a year/time-step iterative approach.
 */
#ifndef LENGTH_INITIALISATIONPHASES_ITERATIVE_H_
#define LENGTH_INITIALISATIONPHASES_ITERATIVE_H_

// headers
#include "../../InitialisationPhases/InitialisationPhase.h"
#include "../../Partition/Accessors/Cached/Categories.h"
#include "../../Partition/Accessors/Categories.h"
#include "../../Processes/Length/RecruitmentBevertonHolt.h"

// namespaces
namespace niwa {
class TimeStep;
namespace initialisationphases {
namespace length {
namespace cached   = partition::accessors::cached;
namespace accessor = partition::accessors;
using processes::length::RecruitmentBevertonHolt;

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
  bool CheckConvergence(unsigned year);

  // members
  unsigned             years_;
  vector<string>       insert_processes_;
  vector<string>       exclude_processes_;
  vector<TimeStep*>    time_steps_;
  bool                 plus_group_;
  cached::Categories   cached_partition_;
  accessor::Categories partition_;

  vector<length::RecruitmentBevertonHolt*> recruitment_process_;
};

}  // namespace length
}  // namespace initialisationphases
} /* namespace niwa */

#endif /* LENGTH_INITIALISATIONPHASES_ITERATIVE_H_ */
