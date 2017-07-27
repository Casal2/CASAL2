/**
 * @file MortalityPreySuitability.h
 * @author C. Marsh
 * @github https://github.com/Zaita
 * @date 1/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This mortality process is a combination of the
 * constant and event mortality processes.
 */
#ifndef SOURCE_PROCESSES_CHILDREN_MORTALITYPREYSUITABILITY_H_
#define SOURCE_PROCESSES_CHILDREN_MORTALITYPREYSUITABILITY_H_

// headers
#include "Common/Model/Model.h"
#include "Common/Partition/Accessors/CombinedCategories.h"
#include "Common/Penalties/Children/Process.h"
#include "Common/Processes/Process.h"
#include "Common/Selectivities/Selectivity.h"
#include "Common/Utilities/Map.h"

// namespaces
namespace niwa {
namespace processes {

using partition::accessors::CombinedCategoriesPtr;


// classes
class MortalityPreySuitability : public Process {
public:
  // methods
  explicit MortalityPreySuitability(Model* model);
  virtual                     ~MortalityPreySuitability() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        DoExecute() override final;


private:
  vector<unsigned>            years_;
  string                      penalty_label_ = "";
  penalties::Process*         penalty_ = nullptr;
  Double                      u_max_;
  vector<string>              prey_category_labels_;
  vector<string>              predator_category_labels_;
  Double                      consumption_rate_;
  vector<Double>              electivities_;
  vector<string>              prey_selectivity_labels_;
  vector<string>              predator_selectivity_labels_;
  vector<Selectivity*>        prey_selectivities_;
  vector<Selectivity*>        predator_selectivities_;
  CombinedCategoriesPtr       prey_partition_;
  CombinedCategoriesPtr       predator_partition_;

};

} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_MORTALITYPREYSUITABILITY_H_ */
