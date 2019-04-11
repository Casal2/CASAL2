/**
 * @file MortalityInstantaneousRetained.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @author I Doonan
 * @github https://github.com/Zaita
 * @date 28/07/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This mortality process is a combination of the
 * constant and event mortality processes and has retained catch (retained_selectivity)
 * i.e., catch on board is discards + retained catch (uses selectivity)
 * Catch in "table catch" is retained catch
 * Currently all discards die and so are removed from the partition << CHANGE LATER
 */
 
#ifndef SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUS_H_
#define SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUS_H_

// headers
#include "Model/Model.h"
#include "Partition/Accessors/Categories.h"
#include "Penalties/Common/Process.h"
#include "Processes/Process.h"
#include "Selectivities/Selectivity.h"
#include "Utilities/Map.h"
#include "AgeWeights/AgeWeight.h"

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
 * could inherit from class MortalityInstantaneous? <<<<<<<<<<<
 * only struct FisheryCategoryData  & FisheryData are changed
 */
  struct FisheryData {
    string          label_;
    string          time_step_label_;
    unsigned        time_step_index_;
    Double          u_max_;
    string          penalty_label_;
    Penalty*        penalty_ = nullptr;
    map<unsigned, Double>  retained_catches_; //landing reported for fishery
    map<unsigned, Double>  actual_retained_catches_;
    map<unsigned, Double>  catches_;         //discard + retained as estimated from retained catch
    map<unsigned, Double>  discards_;         //discard + retained as estimated from retained catch
    map<unsigned, Double>  actual_catches_;  //discard + retained 
    map<unsigned, Double>  actual_discards_; // estimated discard catch
    map<unsigned, Double>  exploitation_by_year_; // I(CM?) added this so it can be reported
													// maybe define for retained catch part of the stock

    Double          vulnerability_;
    Double          retained_vulnerability_;
    Double          uobs_fishery_;  // retained catch only??
    Double          exploitation_;
    Double*         m_;  //which M is this?
  };

  struct CategoryData {
    string                category_label_;
    partition::Category*  category_;
    Double*               m_;
    vector<Double>        exploitation_;
	  vector<Double>        retained_exploitation_;
    vector<Double>        exp_values_;
	  string                selectivity_label_;
    Selectivity*          selectivity_;
    vector<Double>        selectivity_values_; // M-ogive
    vector<Double>        retained_selectivity_values_; // M-ogive
	  AgeWeight*            age_weight_ = nullptr;
    string                age_weight_label_;
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
    vector<Double>    selectivity_values_;  // fishing for category_label_
    string            retained_selectivity_label_;
    Selectivity*      retained_selectivity_ = nullptr;
    vector<Double>    retained_selectivity_values_; // fishing
  };   //add discard mortality later

public:
  // methods
  explicit MortalityInstantaneousRetained(Model* model);
  virtual                     ~MortalityInstantaneousRetained();
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoExecute() override final;
  void                        RebuildCache() override final;
  void                        FillReportCache(ostringstream& cache) override final;
  void                        FillTabularReportCache(ostringstream& cache, bool first_run) override final;
  //
  bool                       check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels);
  bool                       check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods);
  bool                       check_methods_for_removal_obs(vector<string> methods);

  // accessors <<<<<<<<<<<<<<<<<<<<<<<<<<<
  map<unsigned, map<string, map<string, vector<Double>>>>&  catch_at() { return removals_by_year_fishery_category_; };
  map<unsigned, map<string, map<string, vector<Double>>>>&  retained_data() { return retained_by_year_fishery_category_; };

  // set
  vector<unsigned>            set_years();
private:
  map<string, CategoryData*>  category_data_;
  vector<CategoryData>        categories_;

  // members
  vector<string>              category_labels_;
  vector<FisheryCategoryData> fishery_categories_;
  map<string, FisheryData>    fisheries_;
  parameters::Table*          catches_table_ = nullptr; //retained catch
  parameters::Table*          method_table_ = nullptr;

  accessor::Categories        partition_;
  Double                      current_m_ = 0.0;

  // members from mortality event
  //Double                      u_max_ = 0.99; // Now attached to the fishery object
  string                      penalty_label_ = "";
  penalties::Process*         penalty_ = nullptr;
  string                      unit_;
  // members from natural mortality
  vector<Double>              m_input_;
  OrderedMap<string, Double>  m_;
  vector<Double>              time_step_ratios_temp_;
  map<unsigned, Double>       time_step_ratios_;
  vector<string>              selectivity_labels_;      //m_ogives?
  vector<Selectivity*>        selectivities_;
  vector<string>			        retained_selectivity_labels_;
  bool					 	            include_retained_ = false;   //<<<<<<<<<<<<<<<<<<<<<<
  // members for observations
  map<unsigned,  map<string, map<string, vector<Double>>>> removals_by_year_fishery_category_; // Year,  fishery, category
  map<unsigned,  map<string, map<string, vector<Double>>>> retained_by_year_fishery_category_; // Year,  fishery, category

  map<unsigned, map<string, vector<string>>> year_method_category_to_store_; // Year,  fishery, category
  // Members for reporting
  vector<unsigned>            time_steps_to_skip_applying_F_mortaltiy_;
  bool                        use_age_weight_ = true;
  vector<vector<vector<Double>>> removals_by_year_category_age_; // year[year_ndx][category_ndx][age_ndx]
  
			// retaied etc?
		
  vector<vector<Double>>     removals_by_category_age_; // [category_ndx][age_ndx]
  vector<vector<Double>>     discards_by_category_age_; // [category_ndx][age_ndx]
  vector<vector<Double>>     retained_by_category_age_; // [category_ndx][age_ndx]


};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUS_H_ */
