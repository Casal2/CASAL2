/**
 * @file ProcessRemovalsByLengthRetained.h
 * @author  S, Datta
 * @version 1.0
 * @date 06/05/2019
 * @section LICENSE
 *
 * Copyright NIWA Science 2019 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * Date 12.8/15
 */
#ifndef AGE_OBSERVATIONS_PROCESS_REMOVALS_BY_LENGTH_RETAINED_H_
#define AGE_OBSERVATIONS_PROCESS_REMOVALS_BY_LENGTH_RETAINED_H_

// Headers
#include "Observations/Observation.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"
#include "Partition/Accessors/CombinedCategories.h"
#include "Processes/Age/Mortality/MortalityInstantaneousRetained.h"

// Namespace
namespace niwa {
namespace observations {
namespace age {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;
using processes::age::MortalityInstantaneousRetained;

/**
 * Class Definition
 */
class ProcessRemovalsByLengthRetained : public niwa::Observation {
public:
  // Methods
  explicit ProcessRemovalsByLengthRetained(shared_ptr<Model> model);
  virtual ~ProcessRemovalsByLengthRetained();
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final{};
  void PreExecute() override final;
  void Execute() override final;
  void CalculateScore() override final;
  bool HasYear(unsigned year) const override final { return std::find(years_.begin(), years_.end(), year) != years_.end(); }

protected:
  // Members
  vector<unsigned>                years_;
  vector<double>                  length_bins_;
  bool                            length_plus_     = false;  
  // have to be assigned a value to be optional, note in the constructpr the default is model_->length_plus()
  unsigned                        number_bins_ = 0;
  parameters::Table*              obs_table_   = nullptr;
  vector<Double>                  process_error_values_;
  map<unsigned, Double>           process_errors_by_year_;
  string                          method_;
  parameters::Table*              error_values_table_ = nullptr;
  CombinedCategoriesPtr           partition_;
  vector<Double>                  length_results_;
  MortalityInstantaneousRetained* mortality_instantaneous_retained_ = nullptr;
  string                          time_step_label_                  = "";
  string                          process_label_;
  bool                            using_model_length_bins = true;
  vector<int>                     map_local_length_bins_to_global_length_bins_;
  bool                            simulated_data_sum_to_one_;
  bool                            sum_to_one_;

  vector<Double>              numbers_at_age_;
  vector<Double>              numbers_at_length_;
  vector<Double>              expected_values_;
  map<unsigned, map<string, vector<double>>> proportions_;
  map<unsigned, map<string, vector<double>>> error_values_;

  vector<string>          allowed_mortality_types_;


};

} /* namespace age */

} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_PROCESS_REMOVALS_BY_LENGTH_RETAINED_H_ */
