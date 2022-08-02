/**
 * @file MortalityHybrid.h
 * @author C Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This mortality process is taken from Stock Synthesis
 */
#ifndef SOURCE_PROCESSES_CHILDREN_MORTALITYHBRID_H_
#define SOURCE_PROCESSES_CHILDREN_MORTALITYHBRID_H_

// headers
#include "AgeWeights/AgeWeight.h"
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
using utilities::OrderedMap;

// classes
class MortalityHybrid : public Mortality {
  /**
   * FisheryData holds all the information related to a fishery
   */
  struct FisheryData {
    unsigned         fishery_ndx_;  // used to reference which fishery this is. Used when this is accessed via the category fishery struct
    string           label_;
    string           time_step_label_;
    unsigned         time_step_index_;
    Double           init_popes_rate_; //robust A.1.22 of SS appendix
    Double           steep_jointer_; //steep logistic joiner at harvest rate of 0.95
    Double           popes_rate_; // = join1 * temp + (Type(1) - join1) * 0.95;
    Double           init_F_; // current_F = -log(Type(1) - temp1); // initial estimate of F
    Double           final_F_;
    Double           annual_duration_;
    Double           vulnerability_;
    vector<unsigned> years_;
    // These objects want to be a map as more useful for projection methods
    map<unsigned, Double> catches_;
    map<unsigned, Double> actual_catches_;
    map<unsigned, Double> F_by_year_;
  };

  struct CategoryData {
    string               category_label_;
    partition::Category* category_;
    Double*              m_;
    vector<Double>       z_values_by_age_;
    vector<Double>       survivor_with_z_by_age_;
    vector<Double>       exp_values_half_duration_m_;
    string               selectivity_label_;
    Selectivity*         selectivity_;
    vector<Double>       selectivity_values_;
    AgeWeight*           age_weight_ = nullptr;
    string               age_weight_label_;
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
  explicit MortalityHybrid(shared_ptr<Model> model);
  virtual ~MortalityHybrid();
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  void RebuildCache() override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;


private:
  map<string, CategoryData*> category_data_;
  vector<CategoryData>       categories_;

  // members
  vector<FisheryCategoryData> fishery_categories_;
  map<string, FisheryData>    fisheries_;
  parameters::Table*          catches_table_ = nullptr;
  parameters::Table*          method_table_  = nullptr;
  accessor::Categories        partition_;
  Double                      current_m_ = 0.0;
  bool                        is_catch_biomass_ = true;
  vector<vector<Double>>      total_catch_by_year_timestep_;
  // members from mortality event
  string              unit_;
  unsigned            F_iterations_;
  // members from natural mortality
  vector<Double>             m_input_;
  OrderedMap<string, Double> m_;
  Double                     max_F_;
  vector<double>             time_step_ratios_temp_;
  map<unsigned, double>      time_step_ratios_;
  vector<string>             selectivity_labels_;
  vector<Selectivity*>       selectivities_;

  // members for observations
  utilities::Vector4 removals_by_year_fishery_category_;  // process_years_ x method_labs x category_labels_ x age

  // map<unsigned, map<string, map<string, vector<Double>>>> removals_by_year_fishery_category_;  // Year,  fishery, category

  // Members for reporting
  vector<unsigned>               time_steps_to_skip_applying_F_mortality_;
  bool                           use_age_weight_ = true;
  vector<Double>                 annual_duration_by_timestep_;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_MORTALITYHBRID_H_ */
