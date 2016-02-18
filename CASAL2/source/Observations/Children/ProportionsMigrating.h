/**
 * @file ProportionsMigrating.h
 * @author  C. Marsh
 * @version 1.0
 * @date 8/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section This Observation describes the Proportion change from each age class.
 *    This is usually associated with TransitionCategory processes (i.e Migration)
 *    where we want to know the proportion in each age class that has migrated.
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef OBSERVATIONS_PROPORTIONSMIGRATING_H_
#define OBSERVATIONS_PROPORTIONSMIGRATING_H_

// Headers
#include "Observations/Observation.h"

#include "Partition/Accessors/CombinedCategories.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"
#include "AgeingErrors/AgeingError.h"

// Namespace
namespace niwa {
namespace observations {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;


/**
 * Class Definition
 */
class ProportionsMigrating : public niwa::Observation {
public:
  // Methods
  explicit ProportionsMigrating(Model* model);
  virtual                     ~ProportionsMigrating();
  void                        DoValidate() override final;
  virtual void                DoBuild() override;
  void                        DoReset() override final { };
  void                        PreExecute() override final;
  void                        Execute() override final;
  void                        CalculateScore() override final;
  bool                        HasYear(unsigned year) const override final { return std::find(years_.begin(), years_.end(), year) != years_.end(); }

protected:
  // Members
  vector<unsigned>              years_;
  unsigned                      min_age_ = 0;
  unsigned                      max_age_ = 0;
  bool                          age_plus_ = false;
  unsigned                      age_spread_ = 0;
  Double                        delta_ = 0.0;
  parameters::Table*            obs_table_ = nullptr;
  Double                        tolerance_ = 0.0;
  vector<Double>                process_error_values_;
  map<unsigned, Double>         process_errors_by_year_;
  string                        ageing_error_label_;
  parameters::Table*            error_values_table_ = nullptr;
  CachedCombinedCategoriesPtr   cached_partition_;
  CombinedCategoriesPtr         partition_;
  AgeingError*                  ageing_error_ = nullptr;
  vector<Double>                age_results_;
  string                        time_step_label_ = "";

  map<unsigned, map<string, vector<Double>>>  proportions_;
  map<unsigned, map<string, vector<Double>>>  error_values_;
};

} /* namespace observations */
} /* namespace niwa */
#endif /* OBSERVATIONS_PROPORTIONSMIGRATING_H_ */
