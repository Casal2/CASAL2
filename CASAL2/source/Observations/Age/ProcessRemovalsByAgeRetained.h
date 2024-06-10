/**
 * @file ProcessRemovalsByAgeRetained.h
 * @author  S Datta
 * @version 1.0
 * @date 02/05/19
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This observation is a specific process observation class. It is associated with the process type mortality_instantaneous_retained.
 * It calls a catch at object that is created from the process which represents the numbers at age halfway trough the mortality process
 * This class then applies ageing error and converts to a proportion which then gets sent to a likelihood for evaluation.
 *
 */
#ifndef AGE_OBSERVATIONS_REMOVALS_BY_AGE_RETAINED_H_
#define AGE_OBSERVATIONS_REMOVALS_BY_AGE_RETAINED_H_

// Headers
#include "AgeingErrors/AgeingError.h"
#include "Observations/Observation.h"
#include "Partition/Accessors/CombinedCategories.h"
#include "Processes/Age/Mortality/MortalityInstantaneousRetained.h"

// Namespace
namespace niwa {
namespace observations {
namespace age {

using partition::accessors::CombinedCategoriesPtr;
using processes::age::MortalityInstantaneousRetained;

/**
 * Class Definition
 */
class ProcessRemovalsByAgeRetained : public niwa::Observation {
public:
  // Methods
  explicit ProcessRemovalsByAgeRetained(shared_ptr<Model> model);
  virtual ~ProcessRemovalsByAgeRetained();
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
  unsigned                        min_age_    = 0;
  unsigned                        max_age_    = 0;
  bool                            plus_group_ = false;
  unsigned                        age_spread_ = 0;
  parameters::Table*              obs_table_  = nullptr;
  vector<Double>                  process_error_values_;
  map<unsigned, Double>           process_errors_by_year_;
  string                          ageing_error_label_ = "";
  vector<string>                  method_;
  parameters::Table*              error_values_table_ = nullptr;
  CombinedCategoriesPtr           partition_;
  AgeingError*                    ageing_error_ = nullptr;
  vector<Double>                  age_results_;
  MortalityInstantaneousRetained* mortality_instantaneous_retained_ = nullptr;
  vector<string>                  time_step_label_;
  string                          process_label_;
  unsigned                        time_step_to_execute_;
  bool                            simulated_data_sum_to_one_;
  bool                            sum_to_one_;

  map<unsigned, map<string, vector<double>>> proportions_;
  map<unsigned, map<string, vector<double>>> error_values_;

  vector<string>          allowed_mortality_types_;

};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_REMOVALS_BY_AGE_RETAINED_H_ */
