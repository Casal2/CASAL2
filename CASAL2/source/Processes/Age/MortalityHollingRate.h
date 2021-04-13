/**
 * @file MortalityHollingRate.h
 * @author C. Marsh
 * @github https://github.com/Zaita
 * @date 31/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This mortality process is effects prey categories. the amount of removals is related to the
 * abundance of predator's and the paramters in the Hollings equation.
 */
#ifndef SOURCE_PROCESSES_CHILDREN_MORTALITYHOLLINGSRATE_H_
#define SOURCE_PROCESSES_CHILDREN_MORTALITYHOLLINGSRATE_H_

// headers
#include "Model/Model.h"
#include "Partition/Accessors/Categories.h"
#include "Penalties/Common/Process.h"
#include "Processes/Process.h"
#include "Selectivities/Selectivity.h"
#include "Utilities/Map.h"

// namespaces
namespace niwa {
namespace processes {
namespace age {

namespace accessor = niwa::partition::accessors;
using utilities::map2D;

// classes
class MortalityHollingRate : public Process {
public:
  // methods
  explicit MortalityHollingRate(shared_ptr<Model> model);
  virtual                     ~MortalityHollingRate() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        DoExecute() override final;
  void                        FillReportCache(ostringstream& cache) override final;
  void                        FillTabularReportCache(ostringstream& cache, bool first_run) override final;


private:
  vector<unsigned>            years_;
  string                      penalty_label_ = "";
  penalties::Process*         penalty_ = nullptr;
  Double                      u_max_;
  bool                        is_abundance_ = true;
  vector<string>              prey_category_labels_;
  vector<string>              predator_category_labels_;
  Double                      a_;
  Double                      b_;
  Double                      x_;
  vector<string>              prey_selectivity_labels_;
  vector<string>              predator_selectivity_labels_;
  vector<Selectivity*>        prey_selectivities_;
  vector<Selectivity*>        predator_selectivities_;
  accessor::Categories        prey_partition_;
  accessor::Categories        predator_partition_;
  vector<Double>              prey_vulnerability_by_year_;
  vector<Double>              prey_mortality_by_year_;
  vector<Double>              predator_vulnerability_by_year_;

  parameters::Table*          predator_selectivities_table_ = nullptr;
  parameters::Table*          prey_selectivities_table_ = nullptr;
  map<unsigned,vector<Double>> prey_selectivity_by_year_;
  map<unsigned,vector<Double>> predator_selectivity_by_year_;
  bool                        prey_selectivity_by_year_supplied_ = false;
  bool                        predator_selectivity_by_year_supplied_ = false;


};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_MORTALITYHOLLINGSRATE_H_ */
