/**
 * @file MortalityInstantaneous.h
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This mortality process is a combination of the
 * constant and event mortality processes.
 */
#ifndef SOURCE_PROCESSES_LENGTH_CHILDREN_MORTALITYINSTANTANEOUS_H_
#define SOURCE_PROCESSES_LENGTH_CHILDREN_MORTALITYINSTANTANEOUS_H_

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
namespace length {

namespace accessor = niwa::partition::accessors;
using utilities::OrderedMap;

// classes
class MortalityInstantaneous : public Process {
  /**
   * FisheryData holds all the information related to a fishery
   */
  struct FisheryData {
    unsigned         fishery_ndx_;  // used to reference which fishery this is. Used when this is accessed via the category fishery struct
    string           label_;
    string           time_step_label_;
    unsigned         time_step_index_;
    Double           u_max_;
    string           penalty_label_;
    Penalty*         penalty_ = nullptr;
    vector<unsigned> years_;
    // These objects want to be a map as more useful for projection methods
    map<unsigned, Double> catches_;
    map<unsigned, Double> actual_catches_;
    map<unsigned, Double> exploitation_by_year_;

    Double  vulnerability_;
    Double  uobs_fishery_;
    Double  exploitation_;
    Double* m_;
  };

  struct CategoryData {
    string               category_label_;
    partition::Category* category_;
    Double*              m_;
    vector<Double>       exploitation_;
    vector<Double>       exp_values_half_m_;
    string               selectivity_label_;
    Selectivity*         selectivity_;
    vector<Double>       selectivity_values_;
    bool                 used_in_current_timestep_;
    unsigned             category_ndx_;  // used for a look up to store info in containers
  };
  /**
   * FisheryCategoryData is used to store 1 Fishery x Category x Selectivity
   */
  struct FisheryCategoryData {
    FisheryCategoryData(FisheryData& x, CategoryData& y) : fishery_(x), category_(y){};
    FisheryData&   fishery_;
    CategoryData&  category_;
    string         fishery_label_;
    string         category_label_;
    string         selectivity_label_;
    Selectivity*   selectivity_ = nullptr;
    vector<Double> selectivity_values_;
  };

public:
  // methods
  explicit MortalityInstantaneous(shared_ptr<Model> model);
  virtual ~MortalityInstantaneous();
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  void RebuildCache() override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;
  //
  bool             check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels);
  bool             check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods);
  bool             check_methods_for_removal_obs(vector<string> methods);
  vector<unsigned> get_fishery_ndx_for_catch_at(vector<string> fishery_labels);
  vector<unsigned> get_category_ndx_for_catch_at(vector<string> category_labels);
  vector<unsigned> get_year_ndx_for_catch_at(vector<unsigned> years);

  // accessors
  // map<unsigned, map<string, map<string, vector<Double>>>>& catch_at() { return removals_by_year_fishery_category_; };
  vector<Double>& get_catch_at_by_year_fishery_category(unsigned year_ndx, unsigned fishery_ndx, unsigned category_ndx) {
    return removals_by_year_fishery_category_[year_ndx][fishery_ndx][category_ndx];
  };
  const vector<string>& category_labels() const { return category_labels_; }

  // set

private:
  map<string, CategoryData*> category_data_;
  vector<CategoryData>       categories_;

  // members
  vector<string>              category_labels_;
  vector<FisheryCategoryData> fishery_categories_;
  map<string, FisheryData>    fisheries_;
  parameters::Table*          catches_table_ = nullptr;
  parameters::Table*          method_table_  = nullptr;
  accessor::Categories        partition_;
  Double                      current_m_ = 0.0;
  vector<unsigned>            process_years_;  // Can we get @project classes to modify this?
  bool                        is_catch_biomass_ = true;

  // members from mortality event
  // Double                      u_max_ = 0.99; // Now attached to the fishery object
  string              penalty_label_ = "";
  penalties::Process* penalty_       = nullptr;
  string              unit_;

  // members from natural mortality
  vector<Double>             m_input_;
  OrderedMap<string, Double> m_;
  vector<double>             time_step_ratios_temp_;
  map<unsigned, double>      time_step_ratios_;
  vector<string>             selectivity_labels_;
  vector<Selectivity*>       selectivities_;

  // members for observations
  utilities::Vector4 removals_by_year_fishery_category_;  // process_years_ x method_labs x category_labels_ x length

  // map<unsigned, map<string, map<string, vector<Double>>>> removals_by_year_fishery_category_;  // Year,  fishery, category

  // Members for reporting
  vector<unsigned>               time_steps_to_skip_applying_F_mortality_;
  vector<string>                 fishery_labels_;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_LENGTH_CHILDREN_MORTALITYINSTANTANEOUS_H_ */
