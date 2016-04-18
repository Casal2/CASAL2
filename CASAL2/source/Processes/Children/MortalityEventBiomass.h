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
#include "Penalties/Children/Process.h"
#include "Processes/Process.h"
#include "Utilities/Math.h"

// namespaces
namespace niwa {
class Selectivity;

namespace processes {
namespace accessor = niwa::partition::accessors;

/**
 * class definition
 */
class MortalityEventBiomass : public niwa::Process {
public:
  // methods
  explicit MortalityEventBiomass(Model* model);
  virtual                     ~MortalityEventBiomass() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        DoExecute() override final;

private:
  // members
  accessor::Categories        partition_;
  vector<string>              category_labels_;
  vector<string>              selectivity_labels_;
  vector<Selectivity*>        selectivities_;
  Double                      u_max_ = 0;
  Double                      catch_ = 0;
  vector<Double>              catches_;
  map<unsigned, Double>       catch_years_;
  Double                      exploitation_ = 0;
  vector<unsigned>            years_;
  string                      penalty_label_ = "";
  penalties::Process*         penalty_ = nullptr;
  string                      unit_;

};

} /* namespace processes */
} /* namespace niwa */
#endif /* MORTALITYEVENTBIOMASS_H_ */
