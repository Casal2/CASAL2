/**
 * @file ProportionsMatureByAge.h
 * @author  C. Marsh
 * @version 1.0
 * @date 8/10/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section This Observation is the at age maturation class, where users can give proportions mature at age to help estimate maturity ogives
 *
 * The time class represents a moment of time.
 *
 */
#ifndef AGE_OBSERVATIONS_PROPORTIONS_MATURATION_H_
#define AGE_OBSERVATIONS_PROPORTIONS_MATURATION_H_

// Headers
#include "AgeingErrors/AgeingError.h"
#include "Observations/Observation.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"
#include "Partition/Accessors/CombinedCategories.h"

// Namespace
namespace niwa {
namespace observations {
namespace age {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;

/**
 * Class Definition
 */
class ProportionsMatureByAge : public niwa::Observation {
public:
  // Methods
  explicit ProportionsMatureByAge(shared_ptr<Model> model);
  virtual ~ProportionsMatureByAge();
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
  unsigned                    min_age_    = 0;
  unsigned                    max_age_    = 0;
  bool                        plus_group_ = false;
  unsigned                    age_spread_ = 0;
  parameters::Table*          obs_table_  = nullptr;
  vector<Double>              process_error_values_;
  map<unsigned, Double>       process_errors_by_year_;
  string                      ageing_error_label_;
  parameters::Table*          error_values_table_ = nullptr;
  CachedCombinedCategoriesPtr cached_partition_;
  CombinedCategoriesPtr       partition_;
  vector<string>              total_category_labels_;
  CachedCombinedCategoriesPtr total_cached_partition_;
  CombinedCategoriesPtr       total_partition_;
  Double                      time_step_proportion_;
  AgeingError*                ageing_error_ = nullptr;
  vector<Double>              age_results_;
  string                      time_step_label_ = "";

  map<unsigned, map<string, vector<double>>> proportions_;
  map<unsigned, map<string, vector<double>>> error_values_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_PROPORTIONS_MATURATION_H_ */
