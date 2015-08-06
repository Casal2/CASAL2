/**
 * @file MortalityInstantaneous.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/07/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This mortality process is a combination of the
 * constant and event mortality processes.
 */
#ifndef SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUS_H_
#define SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUS_H_

// headers
#include "Model/Model.h"
#include "Partition/Accessors/Categories.h"
#include "Penalties/Children/Process.h"
#include "Processes/Process.h"
#include "Selectivities/Selectivity.h"

// namespaces
namespace niwa {
namespace processes {

namespace accessor = niwa::partition::accessors;

// classes
class MortalityInstantaneous : public Process {
public:
  // methods
  MortalityInstantaneous();
  virtual                     ~MortalityInstantaneous() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        DoExecute() override final;

private:
  // members
  vector<string>              category_labels_;
  vector<unsigned>            years_;
  parameters::TablePtr        catches_table_;
  parameters::TablePtr        fisheries_table_;
  accessor::Categories        partition_;
  ModelPtr                    model_;
  map<string, map<unsigned, Double>>        fishery_by_year_with_catch_;
  map<string, map<string, string>>          fishery_table_data_;
  map<string, map<string, SelectivityPtr>>  fishery_by_category_with_selectivity_;
  // members from mortality event
  Double                                    u_max_;
  string                                    penalty_name_;
//  map<string, map<unsigned, Double> >       category_vulnerable_;
  penalties::ProcessPtr                     penalty_;
  // members from natural mortality
  vector<Double>                            m_;
//  vector<Double>                            ratios_;
//  map<unsigned, Double>                     time_step_ratios_;
//  timesteps::Manager&                       time_steps_manager_;
};

} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUS_H_ */
