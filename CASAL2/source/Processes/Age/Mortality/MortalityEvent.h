/**
 * @file MortalityEvent.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
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
#include "Penalties/Common/Process.h"
#include "Processes/Process.h"
#include "Selectivities/Selectivity.h"
#include "Processes/Age/Mortality.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {

namespace accessor = niwa::partition::accessors;

/**
 * Class definition
 */
class MortalityEvent : public Mortality {
public:
  // Methods
  explicit MortalityEvent(shared_ptr<Model> model);
  virtual ~MortalityEvent() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;

  // inheritetd accessors from the parent Mortality class
  // These should never be called so adding LOG_CODE_ERROR()
  // If they are called you need to change the source code so they are never able to be called.
  bool             check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels){LOG_CODE_ERROR() << "cannot call check_categories_in_methods_for_removal_obs() from MortalityEvent"; return false;};
  bool             check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods){LOG_CODE_ERROR() << "cannot call check_years_in_methods_for_removal_obs() from MortalityEvent"; return false;};
  bool             check_methods_for_removal_obs(vector<string> methods){LOG_CODE_ERROR() << "cannot call check_years_in_methods_for_removal_obs() from MortalityEvent"; return false;};
  vector<unsigned> get_fishery_ndx_for_catch_at(vector<string> fishery_labels){LOG_CODE_ERROR() << "cannot call get_fishery_ndx_for_catch_at() from MortalityEvent"; vector<unsigned> null_result = {1}; return null_result;};
  vector<unsigned> get_category_ndx_for_catch_at(vector<string> category_labels){LOG_CODE_ERROR() << "cannot call get_category_ndx_for_catch_at() from MortalityEvent"; vector<unsigned> null_result = {1}; return null_result;};
  vector<unsigned> get_year_ndx_for_catch_at(vector<unsigned> years){LOG_CODE_ERROR() << "cannot call get_year_ndx_for_catch_at() from MortalityEvent"; vector<unsigned> null_result = {1}; return null_result;};
  // accessors
  vector<Double>& get_catch_at_by_year_fishery_category(unsigned year_ndx, unsigned fishery_ndx, unsigned category_ndx) {
    LOG_CODE_ERROR() << "cannot call get_catch_at_by_year_fishery_category() from MortalityEvent";
    return removals_by_year_fishery_category_[year_ndx][fishery_ndx][category_ndx];};
private:
  // Members
  vector<unsigned> years_;
  vector<Double>   catches_;
  vector<Double>   exploitation_;
  vector<Double>   actual_catches_;

  map<unsigned, Double>               catch_years_;
  double                              u_max_;
  vector<string>                      selectivity_names_;
  string                              penalty_name_;
  accessor::Categories                partition_;
  vector<Selectivity*>                selectivities_;
  map<string, map<unsigned, Double> > vulnerable_;
  penalties::Process*                 penalty_ = nullptr;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* MORTALITYEVENT_H_ */
