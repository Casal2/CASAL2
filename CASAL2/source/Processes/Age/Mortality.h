/**
 * @file Mortality.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Parent class of the mortality children. I split it out so that we can have a common mortality 
 * class that we can get from the manager. Useful now that we have multiple F methods.
 */
#ifndef MORTALITY_H_
#define MORTALITY_H_

// Headers
#include "Processes/Process.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {

/**
 * Class Definition
 * 
 */
class Mortality : public niwa::Process {
public:
  // Methods
  explicit Mortality(shared_ptr<Model> model);
  virtual ~Mortality() = default;
  virtual void DoValidate() {};
  virtual void DoBuild() {};
  virtual void DoReset() {};
  virtual void DoExecute() {};
  virtual void RebuildCache() {};
  virtual void FillReportCache(ostringstream& cache) {};
  virtual void FillTabularReportCache(ostringstream& cache, bool first_run) {};

  // accessors that can be shared across multiple mortality methods
  // These accessors and event members will not be common for all Mortality Children
  // e.g. ConstantRate doesn't have the concept of fisheries and Hollings predatorPrey doesn't have
  // categories or fisheries. However for many of the workhorse mortality classes these will be relevant.
  bool             check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels);
  bool             check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods);
  bool             check_methods_for_removal_obs(vector<string> methods);
  vector<unsigned> get_fishery_ndx_for_catch_at(vector<string> fishery_labels);
  vector<unsigned> get_category_ndx_for_catch_at(vector<string> category_labels);
  vector<unsigned> get_year_ndx_for_catch_at(vector<unsigned> years);
  // accessors
  vector<Double>& get_catch_at_by_year_fishery_category(unsigned year_ndx, unsigned fishery_ndx, unsigned category_ndx) {
    return removals_by_year_fishery_category_[year_ndx][fishery_ndx][category_ndx];
  };
  const vector<string>& category_labels() const { return category_labels_; }
protected:
  // Members
  utilities::Vector4             removals_by_year_fishery_category_;  // process_years_ x method_labs x category_labels_ x age
  vector<string>                 fishery_labels_;
  vector<string>                 category_labels_;
  map<string, vector<string>>    fishery_category_check_; // fishery x categories
  map<string, map<unsigned, Double>>   fishery_catch_; // fishery x  year = catch
  vector<unsigned>               process_years_;  // Can we get @project classes to modify this?

};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */

#endif /* MORTALITY_H_ */
