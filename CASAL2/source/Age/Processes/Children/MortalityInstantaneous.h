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
#include "Common/Model/Model.h"
#include "Common/Partition/Accessors/Categories.h"
#include "Common/Penalties/Children/Process.h"
#include "Common/Processes/Process.h"
#include "Common/Selectivities/Selectivity.h"
#include "Common/Utilities/Map.h"

// namespaces
namespace niwa {
namespace age {
namespace processes {

namespace accessor = niwa::partition::accessors;
using utilities::OrderedMap;

// classes
class MortalityInstantaneous : public Process {
 /**
 * FisheryData holds all the information related to a fishery
 */
  struct FisheryData {
    string          label_;
    string          time_step_label_;
    unsigned        time_step_index_;
    Double          u_max_;
    string          penalty_label_;
    Penalty*        penalty_ = nullptr;
    map<unsigned, Double>  catches_;
    map<unsigned, Double>  actual_catches_;

    Double          vulnerability_;
    Double          uobs_fishery_;
    Double          exploitation_;
    Double*         m_;
  };

  struct CategoryData {
      string                category_label_;
      partition::Category*  category_;
      Double*               m_;
      vector<Double>        exploitation_;
      vector<Double>        exp_values_;
      string                selectivity_label_;
      Selectivity*          selectivity_;
      vector<Double>        selectivity_values_;
      bool                  used_in_current_timestep_;
    };
  /**
   * FisheryCategoryData is used to store 1 Fishery x Category x Selectivity
   */
  struct FisheryCategoryData {
    FisheryCategoryData(FisheryData& x, CategoryData& y) : fishery_(x), category_(y) { };
    FisheryData&      fishery_;
    CategoryData&     category_;
    string            fishery_label_;
    string            category_label_;
    string            selectivity_label_;
    Selectivity*      selectivity_ = nullptr;
    vector<Double>    selectivity_values_;
  };

public:
  // methods
  explicit MortalityInstantaneous(Model* model);
  virtual                     ~MortalityInstantaneous();
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoExecute() override final;
  void                        RebuildCache() override final;
  //
  void                        calculate_requests_from_removal_observation(vector<unsigned> years, vector<string> methods,vector<string> categories);
  // accessors
  map<unsigned, map<string, map<string, vector<Double>>>>&  catch_at() { return removals_by_year_fishery_category_; };

  // set
  vector<unsigned>            set_years();
private:
  map<string, CategoryData*>  category_data_;
  vector<CategoryData>        categories_;

  // members
  vector<string>              category_labels_;
  vector<FisheryCategoryData> fishery_categories_;
  map<string, FisheryData>    fisheries_;
  parameters::Table*          catches_table_ = nullptr;
  parameters::Table*          method_table_ = nullptr;
  accessor::Categories        partition_;
  Double                      current_m_ = 0.0;
//  map<string, Double>         fishery_exploitation;

  // members from mortality event
  Double                      u_max_;
  string                      penalty_label_ = "";
  penalties::Process*         penalty_ = nullptr;
  string                      unit_;
  // members from natural mortality
  vector<Double>              m_input_;
  OrderedMap<string, Double>  m_;
  vector<Double>              time_step_ratios_temp_;
  map<unsigned, Double>       time_step_ratios_;
  vector<string>              selectivity_labels_;
  vector<Selectivity*>        selectivities_;
  // members for observations
  map<unsigned,  map<string, map<string, vector<Double>>>> removals_by_year_fishery_category_; // Year,  fishery, category
  map<unsigned, map<string, vector<string>>> year_method_category_to_store_; // Year,  fishery, category
  // Members for reporting
  unsigned 										reporting_year_ = 0;
  vector<unsigned>            time_steps_to_skip_applying_F_mortaltiy_;
//  map<string, map<unsigned, Double>> category_by_age_with_exploitation;
};

} /* namespace processes */
} /* namespace age */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUS_H_ */
