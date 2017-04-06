/**
 * @file InitialisationMortalityEvent.h
 * @author  C.Marsh
 * @version 1.0
 * @date 6/4/2017
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This is a specific initialisation mortality class. Every iteration in the initialisation phase that this process is executed fish will be killed
 * off by an "event" that involves the user providing the amount as a
 * catch value from the configuration file.
 *
 */
#ifndef INITIALISATIONMORTALITYEVENT_H_
#define INITIALISATIONMORTALITYEVENT_H_

// Headers
#include "Model/Model.h"
#include "Partition/Accessors/Categories.h"
#include "Penalties/Children/Process.h"
#include "Processes/Process.h"
#include "Selectivities/Selectivity.h"

// Namespaces
namespace niwa {
namespace processes {

namespace accessor = niwa::partition::accessors;

/**
 * Class definition
 */
class InitialisationMortalityEvent : public niwa::Process {
public:
  // Methods
  explicit InitialisationMortalityEvent(Model* model);
  virtual                     ~InitialisationMortalityEvent() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        DoExecute() override final;

private:
  // Members
  vector<string>              category_labels_;
  Double                      catch_;
  Double                      u_max_;
  vector<string>              selectivity_names_;
  string                      penalty_name_;
  accessor::Categories        partition_;
  unsigned                    init_iteration_ = 1;
  vector<Selectivity*>        selectivities_;
  map<string, map<unsigned, Double> > vulnerable_;
  penalties::Process*         penalty_ = nullptr;
};

} /* namespace processes */
} /* namespace niwa */
#endif /* INITIALISATIONMORTALITYEVENT_H_ */
