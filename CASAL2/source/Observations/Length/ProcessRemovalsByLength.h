/**
 * @file ProcessRemovalsByLength.h
 * @author  C.Marsh
 * @version 1.0
 * @date 2020
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 */
#ifndef LENGTH_OBSERVATIONS_PROCESS_REMOVALS_BY_LENGTH_H_
#define LENGTH_OBSERVATIONS_PROCESS_REMOVALS_BY_LENGTH_H_

// Headers
#include "Observations/Observation.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"
#include "Partition/Accessors/CombinedCategories.h"
#include "Processes/Length/MortalityInstantaneous.h"

// Namespace
namespace niwa {
namespace observations {
namespace length {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;
using processes::length::MortalityInstantaneous;

/**
 * Class Definition
 */
class ProcessRemovalsByLength : public niwa::Observation {
public:
  // Methods
  explicit ProcessRemovalsByLength(shared_ptr<Model> model);
  virtual ~ProcessRemovalsByLength();
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final{};
  void PreExecute() override final;
  void Execute() override final;
  void CalculateScore() override final;
  bool HasYear(unsigned year) const override final { return std::find(years_.begin(), years_.end(), year) != years_.end(); }

protected:
  // Members
  vector<unsigned>            years_;
  vector<double>              length_bins_;
  bool                        length_plus_;  
  unsigned                    number_bins_ = 0;
  parameters::Table*          obs_table_   = nullptr;
  vector<Double>              process_error_values_;
  map<unsigned, Double>       process_errors_by_year_;
  string                      method_;
  parameters::Table*          error_values_table_ = nullptr;
  CombinedCategoriesPtr       partition_;
  length::MortalityInstantaneous*     mortality_instantaneous_ = nullptr;
  string                      time_step_label_         = "";
  string                      process_label_;
  vector<Double>              expected_values_;
  bool                        using_model_length_bins = true;
  vector<int>                 map_local_length_bins_to_global_length_bins_;
  vector<unsigned>            fishery_ndx_to_get_catch_at_info_;
  vector<unsigned>            year_ndx_to_get_catch_at_info_;
  map<string, unsigned >      category_lookup_for_ndx_to_get_catch_at_info_;
  bool                        simulated_data_sum_to_one_;
  bool                        sum_to_one_;

  map<unsigned, map<string, vector<double>>> proportions_;
  map<unsigned, map<string, vector<double>>> error_values_;
};

} /* namespace length */
} /* namespace observations */
} /* namespace niwa */

#endif /* LENGTH_OBSERVATIONS_PROCESS_REMOVALS_BY_LENGTH_H_ */
