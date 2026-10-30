/**
 * @file MortalityFisheryBase.h
 * @github https://github.com/Zaita
 * @date 30/10/2026
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * Shared setup for the mortality processes that take a catches table and a method table:
 * mortality_instantaneous, mortality_instantaneous_retained and mortality_hybrid. It parses and
 * validates both tables, owns the fishery/category structures and resolves the labels they hold.
 *
 * Parameter binding, parameter validators, DoExecute() and reporting all stay in the child. The
 * columns that belong to one child are handled through the Parse/Check hooks below.
 *
 * Behaviour normalised across the children when this class was introduced (invalid or ambiguous
 * configurations only):
 *  - a negative catch is an error (was mortality_instantaneous only)
 *  - rows of one fishery must agree on u_max and penalty, and mortality_instantaneous_retained
 *    keeps the first row like the others. Rows that disagreed on the penalty silently took the
 *    first row's (the last row's for retained), and retained also took the last row's u_max
 *  - a method table category that is not in the process's categories is an error, not a crash
 *  - a mismatched number of age_weight_label values is an error, not an out of bounds read
 *  - an unknown M ogive is a user error, not a code error
 */
#ifndef SOURCE_PROCESSES_COMMON_MORTALITYFISHERYBASE_H_
#define SOURCE_PROCESSES_COMMON_MORTALITYFISHERYBASE_H_

// headers
#include "AgeWeights/AgeWeight.h"
#include "Model/Model.h"
#include "Partition/Accessors/Categories.h"
#include "Penalties/Common/Process.h"
#include "Processes/Age/Mortality.h"
#include "Selectivities/Selectivity.h"
#include "Utilities/Map.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa::processes::common {

namespace accessor = niwa::partition::accessors;
using utilities::OrderedMap;

/**
 * What differs between the method tables of each child
 */
struct FisheryTableSpec {
  vector<string> extra_required_method_columns = {};     // beyond method, category, selectivity, time_step and penalty
  bool           retained_catches_addressable  = false;  // method_<label> addresses retained_catches_ rather than catches_
};

// classes
class MortalityFisheryBase : public age::Mortality {
public:
  explicit MortalityFisheryBase(shared_ptr<Model> model);
  virtual ~MortalityFisheryBase() = default;
  void DoReset() override final;
  void RebuildCache() override final;

protected:
  /**
   * FisheryData holds all the information related to a fishery
   */
  struct FisheryData {
    unsigned fishery_ndx_     = 0;
    string   label_           = "";
    string   time_step_label_ = "";
    unsigned time_step_index_ = 0;
    Double   u_max_           = 0.0;
    string   penalty_label_   = "";
    Penalty* penalty_         = nullptr;
    // These objects want to be a map as more useful for projection methods
    map<unsigned, Double> catches_              = {};
    map<unsigned, Double> actual_catches_       = {};
    map<unsigned, Double> exploitation_by_year_ = {};

    Double vulnerability_ = 0.0;
    Double uobs_fishery_  = 0.0;
    Double exploitation_  = 0.0;

    // mortality_instantaneous
    map<unsigned, Double> uobs_by_year_     = {};
    bool                  catch_as_biomass_ = false;
    bool                  catch_as_u_       = false;

    // mortality_instantaneous_retained
    map<unsigned, Double> retained_catches_            = {};
    map<unsigned, Double> actual_retained_catches_     = {};
    map<unsigned, Double> discards_                    = {};
    map<unsigned, Double> discards_dead_               = {};  // estimated discard catch which dies
    map<unsigned, Double> total_vulnerable_by_year_    = {};
    map<unsigned, Double> retained_vulnerable_by_year_ = {};
    Double                retained_vulnerability_      = 0.0;
    Double                total_vulnerability_         = 0.0;

    // mortality_hybrid
    Double                annual_duration_ = 0.0;
    Double                init_popes_rate_ = 0.0;  // robust A.1.22 of SS appendix
    Double                steep_jointer_   = 0.0;  // steep logistic joiner at harvest rate of 0.95
    Double                popes_rate_      = 0.0;  // = join1 * temp + (Type(1) - join1) * 0.95;
    Double                init_F_          = 0.0;  // current_F = -log(Type(1) - temp1); // initial estimate of F
    Double                final_F_         = 0.0;
    map<unsigned, Double> F_by_year_       = {};
  };

  struct CategoryData {
    string               category_label_           = "";
    partition::Category* category_                 = nullptr;
    Double*              m_                        = nullptr;
    string               selectivity_label_        = "";
    Selectivity*         selectivity_              = nullptr;
    vector<Double>       selectivity_values_       = {};
    AgeWeight*           age_weight_               = nullptr;
    string               age_weight_label_         = "";
    bool                 used_in_current_timestep_ = false;
    unsigned             category_ndx_             = 0;

    // mortality_instantaneous and mortality_instantaneous_retained
    vector<Double> exploitation_      = {};
    vector<Double> exp_values_half_m_ = {};

    // mortality_instantaneous
    vector<Double> m_values_ = {};  // Age: m_at_age_, Length: not used (computed inline)

    // mortality_hybrid
    vector<Double> z_values_by_age_            = {};
    vector<Double> survivor_with_z_by_age_     = {};
    vector<Double> exp_values_half_duration_m_ = {};
  };

  /**
   * FisheryCategoryData is used to store 1 Fishery x Category x Selectivity
   */
  struct FisheryCategoryData {
    FisheryCategoryData(FisheryData& x, CategoryData& y) : fishery_(x), category_(y) {};
    FisheryData&   fishery_;
    CategoryData&  category_;
    string         fishery_label_      = "";
    string         category_label_     = "";
    string         selectivity_label_  = "";
    Selectivity*   selectivity_        = nullptr;
    vector<Double> selectivity_values_ = {};

    // mortality_instantaneous_retained
    string         retained_selectivity_label_           = "";
    Selectivity*   retained_selectivity_                 = nullptr;
    vector<Double> retained_selectivity_values_          = {};
    string         discard_mortality_selectivity_label_  = "";
    Selectivity*   discard_mortality_selectivity_        = nullptr;
    vector<Double> discard_mortality_selectivity_values_ = {};  // mortality of discards
  };

  /**
   * One row of the method table, with its cells looked up by column name
   */
  struct MethodRow {
    const vector<string>& columns_;
    const vector<string>& cells_;
    unsigned              number_;  // from 1, for error messages
    const string&         operator[](const string& column) const;
  };

  // Called in this order from the child's DoValidate(), after it has validated its parameters
  void BuildFisheryTables(const FisheryTableSpec& spec);

  // Called in this order from the child's DoBuild()
  void     BindCategories();
  void     BuildTimeStepRatios(bool proportions_are_optional);
  void     ResolveFisheries();
  void     ResolveCategories();
  void     BuildSkippedFTimeSteps(const string& process_type);
  void     AllocateRemovals(unsigned bin_count);
  unsigned CategoryBinCount(const CategoryData& category);

  // Hooks for the columns that belong to one child, called for each method table row
  virtual void ParseFisheryColumns(const MethodRow& row, FisheryData& fishery) {}
  virtual void CheckRepeatedFishery(const MethodRow& row, const FisheryData& stored, const FisheryData& repeat, unsigned first_row) {}
  virtual void InitialiseFishery(FisheryData& fishery) {}
  virtual void ParseFisheryCategoryColumns(const MethodRow& row, unsigned category_count, unsigned first_fishery_category) {}

  // Shared by the children with a u_max column
  void ParseUMax(const MethodRow& row, FisheryData& fishery);
  void CheckRepeatedUMax(const MethodRow& row, const FisheryData& stored, const FisheryData& repeat, unsigned first_row);

  // members
  map<string, CategoryData*>  category_data_;
  vector<CategoryData>        categories_;
  vector<FisheryCategoryData> fishery_categories_;
  map<string, FisheryData>    fisheries_;
  parameters::table::Table*   catches_table_ = nullptr;
  parameters::table::Table*   method_table_  = nullptr;
  accessor::Categories        partition_;

  // members from natural mortality
  vector<Double>             m_input_;
  OrderedMap<string, Double> m_;
  vector<double>             time_step_ratios_temp_;
  map<unsigned, double>      time_step_ratios_;
  vector<string>             selectivity_labels_;
  vector<Selectivity*>       selectivities_;

  // Members for reporting
  vector<unsigned> time_steps_to_skip_applying_F_mortality_;
  bool             use_age_weight_ = true;

private:
  map<string, map<unsigned, Double>> ParseCatchesTable();
  void                               BuildCategoryData();
  void                               ParseMethodTable(const FisheryTableSpec& spec, map<string, map<unsigned, Double>>& fishery_year_catch);
};

}  // namespace niwa::processes::common

#endif /* SOURCE_PROCESSES_COMMON_MORTALITYFISHERYBASE_H_ */
