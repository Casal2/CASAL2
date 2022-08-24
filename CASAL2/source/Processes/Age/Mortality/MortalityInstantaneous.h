/**
 * @file MortalityInstantaneous.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/07/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This mortality process is a combination of the
 * constant and event mortality processes.
 */
#ifndef SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUS_H_
#define SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUS_H_

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
class MortalityInstantaneous : public Mortality {
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
    map<unsigned, Double> uobs_by_year_;

    Double  vulnerability_;
    Double  uobs_fishery_;
    Double  exploitation_;
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


  // set

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

  // Members for reporting
  vector<unsigned>               time_steps_to_skip_applying_F_mortality_;
  bool                           use_age_weight_ = true;
  vector<vector<vector<Double>>> removals_by_year_category_age_;  // year[year_ndx][category_ndx][age_ndx]
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUS_H_ */
