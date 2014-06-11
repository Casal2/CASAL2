/**
 * @file MortalityEventBiomass.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 8/11/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef MORTALITYEVENTBIOMASS_H_
#define MORTALITYEVENTBIOMASS_H_

// headers
#include "Model/Model.h"
#include "Partition/Accessors/Categories.h"
#include "Penalties/Penalty.h"
#include "Processes/Process.h"
#include "Selectivities/Selectivity.h"

// namespaces
namespace isam {
namespace processes {

namespace accessor = isam::partition::accessors;

/**
 * class definition
 */
class MortalityEventBiomass : public isam::Process {
public:
  // methods
  MortalityEventBiomass();
  virtual                     ~MortalityEventBiomass() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        PreExecute() override final { };
  void                        Execute() override final;

private:
  // members
  ModelPtr                    model_;
  accessor::Categories        partition_;
  vector<string>              category_labels_;
  vector<string>              selectivity_labels_;
  vector<SelectivityPtr>      selectivities_;
  Double                      u_max_;
  Double                      catch_;
  vector<Double>              catches_;
  map<unsigned, Double>       catch_years_;
  Double                      exploitation_;
  vector<unsigned>            years_;
  string                      penalty_label_;
  PenaltyPtr                  penalty_;
};

} /* namespace processes */
} /* namespace isam */
#endif /* MORTALITYEVENTBIOMASS_H_ */
