/**
 * @file MortalityFisheryAccessorStub.h
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * niwa::processes::age::Mortality exists to give fishery/catch-at-age mortality processes a
 * common base the manager can retrieve them by (see age::Mortality's own doc comment). A
 * handful of non-fishery-based mortality processes -- MortalityConstantRemovalRate (both its
 * mortality_constant_rate and mortality_constant_exploitation formulations) -- derive from it
 * anyway purely so that removal observations naming them as a mortality_process can be
 * dynamic_cast<age::Mortality*> (see Processes/Manager.cpp::GetAgeBasedMortalityProcess()) and
 * fail with the correct diagnostic rather than "process not found".
 *
 * Those classes don't use age::Mortality's real fishery-catch API, so they must override all
 * six of its accessor methods to fail loudly if ever (mis)called. This class centralises that
 * one stub implementation instead of duplicating it per class.
 */
#ifndef PROCESSES_AGE_MORTALITYFISHERYACCESSORSTUB_H_
#define PROCESSES_AGE_MORTALITYFISHERYACCESSORSTUB_H_

// Headers
#include <utility>

#include "Processes/Age/Mortality.h"

// namespaces
namespace niwa::processes::age {

/**
 * Class Definition
 */
class MortalityFisheryAccessorStub : public niwa::processes::age::Mortality {
public:
  // Methods
  MortalityFisheryAccessorStub(shared_ptr<Model> model, string derived_class_name) : Mortality(model), derived_class_name_(std::move(derived_class_name)) {}
  virtual ~MortalityFisheryAccessorStub() = default;

  // inherited accessors from the parent Mortality class
  // These should never be called so adding LOG_CODE_ERROR()
  // If they are called you need to change the source code so they are never able to be called.
  bool check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels) {
    LOG_CODE_ERROR() << "cannot call check_categories_in_methods_for_removal_obs() from " << derived_class_name_;
    return false;
  };
  bool check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods) {
    LOG_CODE_ERROR() << "cannot call check_years_in_methods_for_removal_obs() from " << derived_class_name_;
    return false;
  };
  bool check_methods_for_removal_obs(vector<string> methods) {
    LOG_CODE_ERROR() << "cannot call check_methods_for_removal_obs() from " << derived_class_name_;
    return false;
  };
  vector<unsigned> get_fishery_ndx_for_catch_at(vector<string> fishery_labels) {
    LOG_CODE_ERROR() << "cannot call get_fishery_ndx_for_catch_at() from " << derived_class_name_;
    vector<unsigned> null_result = {1};
    return null_result;
  };
  vector<unsigned> get_category_ndx_for_catch_at(vector<string> category_labels) {
    LOG_CODE_ERROR() << "cannot call get_category_ndx_for_catch_at() from " << derived_class_name_;
    vector<unsigned> null_result = {1};
    return null_result;
  };
  vector<unsigned> get_year_ndx_for_catch_at(vector<unsigned> years) {
    LOG_CODE_ERROR() << "cannot call get_year_ndx_for_catch_at() from " << derived_class_name_;
    vector<unsigned> null_result = {1};
    return null_result;
  };
  // accessors
  vector<Double>& get_catch_at_by_year_fishery_category(unsigned year_ndx, unsigned fishery_ndx, unsigned category_ndx) {
    LOG_CODE_ERROR() << "cannot call get_catch_at_by_year_fishery_category() from " << derived_class_name_;
    return removals_by_year_fishery_category_[year_ndx][fishery_ndx][category_ndx];
  };

private:
  string derived_class_name_;
};

}  // namespace niwa::processes::age
#endif /* PROCESSES_AGE_MORTALITYFISHERYACCESSORSTUB_H_ */
