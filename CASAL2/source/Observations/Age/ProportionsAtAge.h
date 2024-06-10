/**
 * @file ProportionsAtAge.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/04/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef AGE_OBSERVATIONS_PROPORTIONSATAGE_H_
#define AGE_OBSERVATIONS_PROPORTIONSATAGE_H_

// Headers
#include "AgeingErrors/AgeingError.h"
#include "Observations/Observation.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"
#include "Partition/Accessors/CombinedCategories.h"
#include "Processes/Age/Mortality/MortalityInstantaneous.h"

// Namespace
namespace niwa {
class Selectivity;
namespace observations {
namespace age {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;
using processes::age::MortalityInstantaneous;

/**
 * Class Definition
 */
class ProportionsAtAge : public niwa::Observation {
public:
  // Methods
  explicit ProportionsAtAge(shared_ptr<Model> model);
  virtual ~ProportionsAtAge();
  void         DoValidate() override final;
  virtual void DoBuild() override;
  void         DoReset() override final{};
  void         PreExecute() override final;
  void         Execute() override final;
  void         CalculateScore() override final;
  bool         HasYear(unsigned year) const override final { return std::find(years_.begin(), years_.end(), year) != years_.end(); }

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
  AgeingError*                ageing_error_ = nullptr;
  vector<Double>              age_results_;
  MortalityInstantaneous*     mortality_instantaneous_ = nullptr;
  vector<string>              selectivity_labels_;
  vector<Selectivity*>        selectivities_;
  string                      time_step_label_                     = "";
  bool                        selectivity_for_combined_categories_ = false;
  vector<Double>              expected_values_;
  vector<Double>              numbers_age_;
  vector<Double>              numbers_at_age_with_error_;
  bool                        simulated_data_sum_to_one_;
  bool                        sum_to_one_;

  map<unsigned, map<string, vector<double>>> proportions_;
  map<unsigned, map<string, vector<double>>> error_values_;
};

} /* namespace age */
}  // namespace observations
}  // namespace niwa
#endif /* AGE_OBSERVATIONS_PROPORTIONSATAGE_H_ */