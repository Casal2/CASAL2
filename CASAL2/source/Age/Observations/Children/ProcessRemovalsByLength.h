/**
 * @file ProcessRemovalsByLength.h
 * @author  C.Marsh
 * @version 1.0
 * @date 8/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science 2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * Date 12.8/15
 */
#ifndef AGE_OBSERVATIONS_PROCESS_REMOVALS_BY_LENGTH_H_
#define AGE_OBSERVATIONS_PROCESS_REMOVALS_BY_LENGTH_H_

// Headers
#include "Common/Observations/Observation.h"
#include "Common/Partition/Accessors/CombinedCategories.h"
#include "Common/Partition/Accessors/Cached/CombinedCategories.h"
#include "Age/Processes/Children/MortalityInstantaneous.h"
// TODO: Move this to age

// Namespace
namespace niwa {
namespace age {
namespace observations {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;
using age::processes::MortalityInstantaneous;

/**
 * Class Definition
 */
class ProcessRemovalsByLength : public niwa::Observation {
public:
  // Methods
  explicit ProcessRemovalsByLength(Model* model);
  virtual                     ~ProcessRemovalsByLength();
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
  MortalityInstantaneous*       mortality_instantaneous_ = nullptr;
  string                        time_step_label_ = "";
  string                        process_label_;

  map<unsigned, map<string, vector<Double>>> proportions_;
  map<unsigned, map<string, vector<Double>>> error_values_;
};

} /* namespace observations */
} /* namespace age */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_PROCESS_REMOVALS_BY_LENGTH_H_ */
