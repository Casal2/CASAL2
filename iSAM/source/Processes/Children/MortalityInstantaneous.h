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
#include "Utilities/Map.h"

// namespaces
namespace niwa {
namespace processes {

namespace accessor = niwa::partition::accessors;
using utilities::map3D;
using utilities::map2D;

// classes
class MortalityInstantaneous : public Process {
  /**
   * Fishery data is used to store 1 Fishery x Category x TimeStep
   * All of the information to populate the FisheryData is taken
   * from the fisheries and catches tables.
   */
  struct FisheryData {
    string          name_;
    string          time_step_label_;
    unsigned        time_step_index_;
    string          category_label_;
    string          selectivity_label_;
    SelectivityPtr  selectivity_;
    Double          u_max_;
    string          penalty_label_;
    PenaltyPtr      penalty_;
    map<unsigned, Double>  catches_;
  };
public:
  // methods
  MortalityInstantaneous();
  virtual                     ~MortalityInstantaneous() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        DoExecute() override final;

  // accessors
  Double                      m(const string& label, unsigned age);
  Double                      time_step_ratio();
  Double                      fishery_exploitation_fraction(const string& fishery_label, const string& category_label, unsigned age);
  bool                        validate_fishery_category(const string& fishery_label, const string& category_label);

private:
  // members
  vector<string>              category_labels_;
  vector<FisheryData>         fisheries_;
  parameters::TablePtr        catches_table_;
  parameters::TablePtr        fisheries_table_;
  accessor::Categories        partition_;
  Double                      current_m_ = 0.0;
  map<string, Double>         fishery_exploitation;
  // members from mortality event
  Double                      u_max_;
  string                      penalty_name_ = "";
  penalties::ProcessPtr       penalty_;
  // members from natural mortality
  vector<Double>              m_;
  vector<Double>              time_step_ratios_temp_;
  map<unsigned, Double>       time_step_ratios_;
  vector<string>              selectivity_names_;
  vector<SelectivityPtr>      selectivities_;
};

} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUS_H_ */
