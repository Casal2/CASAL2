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
#include "Partition/Accessors/CombinedCategories.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"
#include "Processes/Age/MortalityInstantaneousRetained.h"

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
  explicit ProcessRemovalsByLengthRetained(Model* model);
  virtual                     ~ProcessRemovalsByLengthRetained();
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
  vector<Double>                length_bins_;
  bool                          length_plus_ = false;
  unsigned                      number_bins_ = 0;
  parameters::Table*            obs_table_ = nullptr;
  Double                        tolerance_ = 0.0;
  vector<Double>                process_error_values_;
  map<unsigned, Double>         process_errors_by_year_;
  string                        method_;
  parameters::Table*            error_values_table_ = nullptr;
  CachedCombinedCategoriesPtr   cached_partition_;
  CombinedCategoriesPtr         partition_;
  vector<Double>                length_results_;
  MortalityInstantaneousRetained*       mortality_instantaneous_retained_ = nullptr;
  string                        time_step_label_ = "";
  string                        process_label_;

  map<unsigned, map<string, vector<Double>>> proportions_;
  map<unsigned, map<string, vector<Double>>> error_values_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_PROCESS_REMOVALS_BY_LENGTH_RETAINED_H_ */
