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
#include "Processes/Age/Mortality.h"

// namespaces
namespace niwa {
namespace processes {
namespace age {

namespace accessor = niwa::partition::accessors;
using utilities::map2D;

// classes
class MortalityHollingRate : public Mortality {
public:
  // methods
  explicit MortalityHollingRate(shared_ptr<Model> model);
  virtual ~MortalityHollingRate() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final{};
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;
  // inheritetd accessors from the parent Mortality class
  // These should never be called so adding LOG_CODE_ERROR()
  // If they are called you need to change the source code so they are never able to be called.
  bool             check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels){LOG_CODE_ERROR() << "cannot call check_categories_in_methods_for_removal_obs() from MortalityHollingRate"; return false;};
  bool             check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods){LOG_CODE_ERROR() << "cannot call check_years_in_methods_for_removal_obs() from MortalityHollingRate"; return false;};
  bool             check_methods_for_removal_obs(vector<string> methods){LOG_CODE_ERROR() << "cannot call check_years_in_methods_for_removal_obs() from MortalityHollingRate"; return false;};
  vector<unsigned> get_fishery_ndx_for_catch_at(vector<string> fishery_labels){LOG_CODE_ERROR() << "cannot call get_fishery_ndx_for_catch_at() from MortalityHollingRate"; vector<unsigned> null_result = {1}; return null_result;};
  vector<unsigned> get_category_ndx_for_catch_at(vector<string> category_labels){LOG_CODE_ERROR() << "cannot call get_category_ndx_for_catch_at() from MortalityHollingRate"; vector<unsigned> null_result = {1}; return null_result;};
  vector<unsigned> get_year_ndx_for_catch_at(vector<unsigned> years){LOG_CODE_ERROR() << "cannot call get_year_ndx_for_catch_at() from MortalityHollingRate"; vector<unsigned> null_result = {1}; return null_result;};
  // accessors
  vector<Double>& get_catch_at_by_year_fishery_category(unsigned year_ndx, unsigned fishery_ndx, unsigned category_ndx) {
    LOG_CODE_ERROR() << "cannot call get_catch_at_by_year_fishery_category() from MortalityHollingRate";
    return removals_by_year_fishery_category_[year_ndx][fishery_ndx][category_ndx];};
private:
  vector<unsigned>     years_;
  string               penalty_label_ = "";
  penalties::Process*  penalty_       = nullptr;
  double               u_max_;
  bool                 is_abundance_ = true;
  vector<string>       prey_category_labels_;
  vector<string>       predator_category_labels_;
  Double               a_;
  Double               b_;
  Double               x_;
  vector<string>       prey_selectivity_labels_;
  vector<string>       predator_selectivity_labels_;
  vector<Selectivity*> prey_selectivities_;
  vector<Selectivity*> predator_selectivities_;
  accessor::Categories prey_partition_;
  accessor::Categories predator_partition_;
  vector<Double>       prey_vulnerability_by_year_;
  vector<Double>       prey_mortality_by_year_;
  vector<Double>       predator_vulnerability_by_year_;

  parameters::Table*            predator_selectivities_table_ = nullptr;
  parameters::Table*            prey_selectivities_table_     = nullptr;
  map<unsigned, vector<Double>> prey_selectivity_by_year_;
  map<unsigned, vector<Double>> predator_selectivity_by_year_;
  bool                          prey_selectivity_by_year_supplied_     = false;
  bool                          predator_selectivity_by_year_supplied_ = false;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_MORTALITYHOLLINGSRATE_H_ */
