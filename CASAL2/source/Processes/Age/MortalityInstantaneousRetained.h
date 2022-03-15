/**
 * @file MortalityInstantaneousRetained.h
 * @author Samik Datta and Craig Marsh (samik.datta@niwa.co.nz)
 * @github https://github.com/Zaita
 * @date 26/06/2019
 * @section LICENSE
 *
 * Copyright NIWA Science 2019 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This mortality process is a combination of the
 * constant and event mortality processes, with retained and discard mortality processes.
 */
#ifndef SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUSRETAINED_H_
#define SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUSRETAINED_H_

// headers
#include "AgeWeights/AgeWeight.h"
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
using utilities::OrderedMap;

// classes
class MortalityInstantaneousRetained : public Process {
  /**
   * FisheryData holds all the information related to a fishery
   */
  struct FisheryData {
    string   label_;
    string   time_step_label_;
    unsigned time_step_index_;
    double   u_max_;
    string   penalty_label_;
    Penalty* penalty_ = nullptr;

    map<unsigned, Double> catches_;
    map<unsigned, Double> actual_catches_;  // want these two to be TOTAL catches
    map<unsigned, Double> retained_catches_;
    map<unsigned, Double> actual_retained_catches_;  // want these two to be RETAINED catches
    map<unsigned, Double> discards_;
    map<unsigned, Double> discards_dead_;                // estimated discard catch which dies
    map<unsigned, Double> exploitation_by_year_;         // I(CM?) added this so it can be reported
    map<unsigned, Double> total_vulnerable_by_year_;     // I added this so it can be reported
    map<unsigned, Double> retained_vulnerable_by_year_;  // I added this so it can be reported

    Double retained_vulnerability_;
    Double total_vulnerability_;
    Double uobs_fishery_;
    Double exploitation_;
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
    AgeWeight*           age_weight_ = nullptr;
    string               age_weight_label_;
    bool                 used_in_current_timestep_;
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
    string         retained_selectivity_label_;
    Selectivity*   retained_selectivity_ = nullptr;
    vector<Double> retained_selectivity_values_;
    string         discard_mortality_selectivity_label_;
    Selectivity*   discard_mortality_selectivity_ = nullptr;
    vector<Double> discard_mortality_selectivity_values_;  // mortality of discards
  };

public:
  // methods
  explicit MortalityInstantaneousRetained(shared_ptr<Model> model);
  virtual ~MortalityInstantaneousRetained();
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  void RebuildCache() override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;
  //
  bool check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels);
  bool check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods);
  bool check_methods_for_removal_obs(vector<string> methods);

  // accessors
  map<unsigned, map<string, map<string, vector<Double>>>>& catch_at() { return removals_by_year_fishery_category_; };
  map<unsigned, map<string, map<string, vector<Double>>>>& retained_data() { return retained_by_year_fishery_category_; };
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

  accessor::Categories partition_;
  Double               current_m_ = 0.0;
  vector<unsigned>     process_years_;

  // members from mortality event
  // Double                     u_max_ = 0.99; // Now attached to the fishery object
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
  vector<string>             retained_selectivity_labels_;

  // members for observations
  map<unsigned, map<string, map<string, vector<Double>>>> removals_by_year_fishery_category_;           // Year,  fishery, category
  map<unsigned, map<string, map<string, vector<Double>>>> retained_by_year_fishery_category_;           // Year,  fishery, category
  map<unsigned, map<string, map<string, vector<Double>>>> discards_by_year_fishery_category_;           // Year,  fishery, category
  map<unsigned, map<string, map<string, vector<Double>>>> discard_mortality_by_year_fishery_category_;  // Year,  fishery, category

  map<unsigned, map<string, vector<string>>> year_method_category_to_store_;  // Year,  fishery, category

  // Members for reporting
  vector<unsigned>               time_steps_to_skip_applying_F_mortality_;
  bool                           use_age_weight_ = true;
  vector<vector<vector<Double>>> removals_by_year_category_age_;  // year[year_ndx][category_ndx][age_ndx]

  vector<vector<Double>> removals_by_category_age_;  // [category_ndx][age_ndx]
  vector<vector<Double>> retained_by_category_age_;  // [category_ndx][age_ndx]
  vector<vector<Double>> discards_by_category_age_;  // [category_ndx][age_ndx]
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUSRETAINED_H_ */
