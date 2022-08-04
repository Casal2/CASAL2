/**
 * @file MortalityInitialisationBaranov.h
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science �2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This is a mortality class that allows to apply an instaneous F, that is estimable with a selectivity for a set of categories
 *
 */
#ifndef MORTALITY_INITIALISATION_BARANOV_H_
#define MORTALITY_INITIALISATION_BARANOV_H_

// Headers
#include "Model/Model.h"
#include "Partition/Accessors/Categories.h"
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
class MortalityInitialisationBaranov : public Mortality {
public:
  // Methods
  explicit MortalityInitialisationBaranov(shared_ptr<Model> model);
  virtual ~MortalityInitialisationBaranov() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;
  // inheritetd accessors from the parent Mortality class
  // These should never be called so adding LOG_CODE_ERROR()
  // If they are called you need to change the source code so they are never able to be called.
  bool             check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels){LOG_CODE_ERROR() << "cannot call check_categories_in_methods_for_removal_obs() from MortalityInitialisationBaranov"; return false;};
  bool             check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods){LOG_CODE_ERROR() << "cannot call check_years_in_methods_for_removal_obs() from MortalityInitialisationBaranov"; return false;};
  bool             check_methods_for_removal_obs(vector<string> methods){LOG_CODE_ERROR() << "cannot call check_years_in_methods_for_removal_obs() from MortalityInitialisationBaranov"; return false;};
  vector<unsigned> get_fishery_ndx_for_catch_at(vector<string> fishery_labels){LOG_CODE_ERROR() << "cannot call get_fishery_ndx_for_catch_at() from MortalityInitialisationBaranov"; vector<unsigned> null_result = {1}; return null_result;};
  vector<unsigned> get_category_ndx_for_catch_at(vector<string> category_labels){LOG_CODE_ERROR() << "cannot call get_category_ndx_for_catch_at() from MortalityInitialisationBaranov"; vector<unsigned> null_result = {1}; return null_result;};
  vector<unsigned> get_year_ndx_for_catch_at(vector<unsigned> years){LOG_CODE_ERROR() << "cannot call get_year_ndx_for_catch_at() from MortalityInitialisationBaranov"; vector<unsigned> null_result = {1}; return null_result;};
  // accessors
  vector<Double>& get_catch_at_by_year_fishery_category(unsigned year_ndx, unsigned fishery_ndx, unsigned category_ndx) {
    LOG_CODE_ERROR() << "cannot call get_catch_at_by_year_fishery_category() from MortalityInitialisationBaranov";
    return removals_by_year_fishery_category_[year_ndx][fishery_ndx][category_ndx];};
private:
  // Members
  Double                              actual_catch_;

  Double                              f_;
  vector<string>                      selectivity_names_;
  accessor::Categories                partition_;
  vector<Selectivity*>                selectivities_;

};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* MORTALITY_INITIALISATION_BARANOV_H_ */
