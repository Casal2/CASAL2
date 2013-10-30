/**
 * @file MortalityEvent.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This is a time-varying mortality class. Every year fish will be killed
 * off by an "event" that involves the user providing the amount as a
 * catch value from the configuration file.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef MORTALITYEVENT_H_
#define MORTALITYEVENT_H_

// Headers
#include "Model/Model.h"
#include "Partition/Accessors/Categories.h"
#include "Penalties/Penalty.h"
#include "Processes/Process.h"
#include "Selectivities/Selectivity.h"

// Namespaces
namespace isam {
namespace processes {

namespace accessor = isam::partition::accessors;

/**
 * Class definition
 */
class MortalityEvent : public isam::Process {
public:
  // Methods
  MortalityEvent();
  virtual                     ~MortalityEvent() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        PreExecute() override final { };
  void                        Execute() override final;

private:
  // Members
  vector<string>              category_names_;
  vector<unsigned>            years_;
  vector<double>              catches_;
  map<unsigned, Double>       catch_years_;
  Double                      u_max_;
  vector<string>              selectivity_names_;
  string                      penalty_name_;
  accessor::CategoriesPtr     partition_;
  ModelPtr                    model_;
  vector<SelectivityPtr>      selectivities_;
  map<string, map<unsigned, Double> > vulnerable_;
  PenaltyPtr                  penalty_;
};

} /* namespace processes */
} /* namespace isam */
#endif /* MORTALITYEVENT_H_ */
