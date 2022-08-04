/**
 * @file MortalityConstantRate.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a constant annual mortality rate class. Every year
 * this process is executed fish will be killed off at a constant rate.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef MORTALITYCONSTANTRATE_H_
#define MORTALITYCONSTANTRATE_H_

// Headers
#include "Partition/Accessors/Categories.h"
#include "Processes/Process.h"
#include "Processes/Age/Mortality.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {
class Selectivity;
namespace processes {
namespace age {
namespace accessor = niwa::partition::accessors;
using utilities::OrderedMap;
/**
 * Class Definition
 */
class MortalityConstantRate : public Mortality {
public:
  // Methods
  MortalityConstantRate(shared_ptr<Model> model);
  virtual ~MortalityConstantRate() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;
  // inheritetd accessors from the parent Mortality class
  // These should never be called so adding LOG_CODE_ERROR()
  // If they are called you need to change the source code so they are never able to be called.
  bool             check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels){LOG_CODE_ERROR() << "cannot call check_categories_in_methods_for_removal_obs() from MortalityConstantRate"; return false;};
  bool             check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods){LOG_CODE_ERROR() << "cannot call check_years_in_methods_for_removal_obs() from MortalityConstantRate"; return false;};
  bool             check_methods_for_removal_obs(vector<string> methods){LOG_CODE_ERROR() << "cannot call check_years_in_methods_for_removal_obs() from MortalityConstantRate"; return false;};
  vector<unsigned> get_fishery_ndx_for_catch_at(vector<string> fishery_labels){LOG_CODE_ERROR() << "cannot call get_fishery_ndx_for_catch_at() from MortalityConstantRate"; vector<unsigned> null_result = {1}; return null_result;};
  vector<unsigned> get_category_ndx_for_catch_at(vector<string> category_labels){LOG_CODE_ERROR() << "cannot call get_category_ndx_for_catch_at() from MortalityConstantRate"; vector<unsigned> null_result = {1}; return null_result;};
  vector<unsigned> get_year_ndx_for_catch_at(vector<unsigned> years){LOG_CODE_ERROR() << "cannot call get_year_ndx_for_catch_at() from MortalityConstantRate"; vector<unsigned> null_result = {1}; return null_result;};
  // accessors
  vector<Double>& get_catch_at_by_year_fishery_category(unsigned year_ndx, unsigned fishery_ndx, unsigned category_ndx) {
    LOG_CODE_ERROR() << "cannot call get_catch_at_by_year_fishery_category() from MortalityConstantRate";
    return removals_by_year_fishery_category_[year_ndx][fishery_ndx][category_ndx];};
private:
  // Members
  vector<Double>             m_input_;
  OrderedMap<string, Double> m_;
  vector<double>             ratios_;
  map<unsigned, double>      time_step_ratios_;
  vector<vector<Double>>     mortality_rates_;
  vector<string>             selectivity_names_;
  accessor::Categories       partition_;
  vector<Selectivity*>       selectivities_;
  vector<Double>             total_removals_by_year_;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* MORTALITYCONSTANTRATE_H_ */
