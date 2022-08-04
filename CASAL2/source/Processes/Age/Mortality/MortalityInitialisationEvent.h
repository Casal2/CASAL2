/**
 * @file MortalityInitialisationEvent.h
 * @author  C.Marsh
 * @version 1.0
 * @date 6/4/2017
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This is a specific initialisation mortality class. Every iteration in the initialisation phase that this process is executed fish will be killed
 * off by an "event" that involves the user providing the amount as a
 * catch value from the configuration file.
 *
 */
#ifndef INITIALISATIONMORTALITYEVENT_H_
#define INITIALISATIONMORTALITYEVENT_H_

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
class MortalityInitialisationEvent : public Mortality {
public:
  // Methods
  explicit MortalityInitialisationEvent(shared_ptr<Model> model);
  virtual ~MortalityInitialisationEvent() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final{};
  void DoExecute() override final;

  void FillReportCache(ostringstream& cache) override final{};
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final{};
  // inheritetd accessors from the parent Mortality class
  // These should never be called so adding LOG_CODE_ERROR()
  // If they are called you need to change the source code so they are never able to be called.
  bool             check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels){LOG_CODE_ERROR() << "cannot call check_categories_in_methods_for_removal_obs() from MortalityInitialisationEvent"; return false;};
  bool             check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods){LOG_CODE_ERROR() << "cannot call check_years_in_methods_for_removal_obs() from MortalityInitialisationEvent"; return false;};
  bool             check_methods_for_removal_obs(vector<string> methods){LOG_CODE_ERROR() << "cannot call check_years_in_methods_for_removal_obs() from MortalityInitialisationEvent"; return false;};
  vector<unsigned> get_fishery_ndx_for_catch_at(vector<string> fishery_labels){LOG_CODE_ERROR() << "cannot call get_fishery_ndx_for_catch_at() from MortalityInitialisationEvent"; vector<unsigned> null_result = {1}; return null_result;};
  vector<unsigned> get_category_ndx_for_catch_at(vector<string> category_labels){LOG_CODE_ERROR() << "cannot call get_category_ndx_for_catch_at() from MortalityInitialisationEvent"; vector<unsigned> null_result = {1}; return null_result;};
  vector<unsigned> get_year_ndx_for_catch_at(vector<unsigned> years){LOG_CODE_ERROR() << "cannot call get_year_ndx_for_catch_at() from MortalityInitialisationEvent"; vector<unsigned> null_result = {1}; return null_result;};
  // accessors
  vector<Double>& get_catch_at_by_year_fishery_category(unsigned year_ndx, unsigned fishery_ndx, unsigned category_ndx) {
    LOG_CODE_ERROR() << "cannot call get_catch_at_by_year_fishery_category() from MortalityInitialisationEvent";
    return removals_by_year_fishery_category_[year_ndx][fishery_ndx][category_ndx];};
private:
  // Members
  Double                              catch_;
  double                              u_max_;
  vector<string>                      selectivity_names_;
  string                              penalty_name_;
  accessor::Categories                partition_;
  unsigned                            init_iteration_ = 1;
  vector<Selectivity*>                selectivities_;
  map<string, map<unsigned, Double> > vulnerable_;
  penalties::Process*                 penalty_ = nullptr;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* INITIALISATIONMORTALITYEVENT_H_ */
