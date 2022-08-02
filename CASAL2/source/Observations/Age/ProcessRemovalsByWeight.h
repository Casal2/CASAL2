/**
 * @file ProcessRemovalsByWeight.h
 *
 * @section LICENSE
 *
 * Copyright NIWA Science 2021 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 */
#ifndef AGE_OBSERVATIONS_PROCESS_REMOVALS_BY_WEIGHT_H_
#define AGE_OBSERVATIONS_PROCESS_REMOVALS_BY_WEIGHT_H_

// Headers
#include "Observations/Observation.h"
#include "Partition/Accessors/CombinedCategories.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"
#include "Processes/Age/Mortality/MortalityInstantaneous.h"

// Namespace
namespace niwa {
namespace observations {
namespace age {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;
using processes::age::MortalityInstantaneous;

/**
 * Class Definition
 */
class ProcessRemovalsByWeight : public niwa::Observation {
public:
  // Methods
  explicit ProcessRemovalsByWeight(shared_ptr<Model> model);
  virtual                     ~ProcessRemovalsByWeight();
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        PreExecute() override final;
  void                        Execute() override final;
  void                        CalculateScore() override final;
  bool                        HasYear(unsigned year) const override final { return std::find(years_.begin(), years_.end(), year) != years_.end(); }

protected:
  // Members
  vector<unsigned>              years_;
  vector<double>                length_bins_;
  vector<double>                length_bins_plus_;
  vector<double>                length_bins_n_;
  vector<double>                weight_bins_;
  vector<double>                weight_bins_plus_;
  bool                          length_plus_ = false;
  bool                          weight_plus_ = false;
  unsigned                      number_length_bins_ = 0;
  unsigned                      number_weight_bins_ = 0;
  Double                        length_weight_cv_ = 0.0;
  string                        length_weight_distribution_label_;
  Distribution                  length_weight_distribution_;
  string                        units_;
  Double                        fishbox_weight_ = 0.0;
  parameters::Table*            obs_table_ = nullptr;
  vector<Double>                process_error_values_;
  map<unsigned, Double>         process_errors_by_year_;
  string                        method_;
  parameters::Table*            error_values_table_ = nullptr;
  CombinedCategoriesPtr         partition_;
  vector<Double>                length_results_;
  vector<Double>                weight_results_;
  MortalityInstantaneous*       mortality_instantaneous_ = nullptr;
  string                        time_step_label_ = "";
  string                        process_label_;
  Double                        unit_multiplier_;
  // local; here so that it doesn't get reallocated in Execute()
  vector<Double>                                          length_weight_cv_adj_;
  map<string, map<string, vector<vector<Double>>>>        map_age_length_matrix_;
  map<string, map<string, vector<vector<Double>>>>        map_length_weight_matrix_;
  map<string, map<string, vector<vector<Double>>>>        map_age_weight_matrix_;

  vector<unsigned>            fishery_ndx_to_get_catch_at_info_;
  vector<unsigned>            year_ndx_to_get_catch_at_info_;
  map<string, unsigned >      category_lookup_for_ndx_to_get_catch_at_info_;
  
  map<unsigned, map<string, vector<double>>> proportions_;
  map<unsigned, map<string, vector<double>>> error_values_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_PROCESS_REMOVALS_BY_WEIGHT_H_ */
