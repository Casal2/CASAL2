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
#include "InitialisationPhases/InitialisationPhase.h"
#include "Partition/Accessors/Cached/Categories.h"
#include "Partition/Accessors/Categories.h"

// namespaces
namespace niwa {
class TimeStep;
class DerivedQuantity;

namespace initialisationphases {
namespace cached   = partition::accessors::cached;
namespace accessor = partition::accessors;

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
  unsigned                    ssb_offset_ = 4;
};

} /* namespace initialisationphases */
} /* namespace niwa */
#endif /* INITIALISATIONPHASES_DERIVED_H_ */
