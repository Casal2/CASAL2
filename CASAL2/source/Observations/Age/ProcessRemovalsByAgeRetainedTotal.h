/**
 * @file ProcessRemovalsByAgeRetainedTotal.h
 * @author  S Datta
 * @version 1.0
 * @date 02/05/19
 * @section LICENSE
 *
 * Copyright NIWA Science 2019 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This observation is a specific process observation class. It is associated with the process type mortality_instantaneous.
 * It calls a catch at object that is created from the process which represents the numbers at age halfway trough the mortality process
 * This class then applies ageing error and converts to a proportion which then gets sent to a likelihood for evaluation.
 *
 */
#ifndef AGE_OBSERVATIONS_REMOVALS_BY_AGE_RETAINED_TOTAL_H_
#define AGE_OBSERVATIONS_REMOVALS_BY_AGE_RETAINED_TOTAL_H_

// Headers
#include "Observations/Observation.h"

#include "Partition/Accessors/CombinedCategories.h"
#include "Processes/Age/MortalityInstantaneousRetained.h"
#include "AgeingErrors/AgeingError.h"

// Namespace
namespace niwa {
namespace observations {
namespace age {

using partition::accessors::CombinedCategoriesPtr;
using processes::age::MortalityInstantaneousRetained;

/**
 * Class Definition
 */
class ProcessRemovalsByAgeRetainedTotal : public niwa::Observation {
public:
  // Methods
  explicit ProcessRemovalsByAgeRetainedTotal(Model* model);
  virtual                     ~ProcessRemovalsByAgeRetainedTotal();
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
  unsigned                      min_age_ = 0;
  unsigned                      max_age_ = 0;
  bool                          plus_group_ = false;
  unsigned                      age_spread_ = 0;
  parameters::Table*            obs_table_ = nullptr;
  double                        tolerance_ = 0.0;
  vector<Double>                process_error_values_;
  map<unsigned, Double>         process_errors_by_year_;
  string                        ageing_error_label_ = "";
  vector<string>                method_;
  parameters::Table*            error_values_table_ = nullptr;
  CombinedCategoriesPtr         partition_;
  AgeingError*                  ageing_error_ = nullptr;
  vector<Double>                age_results_;
  MortalityInstantaneousRetained*       mortality_instantaneous_retained_ = nullptr;
  vector<string>                time_step_label_;
  string                        process_label_;
  unsigned                      time_step_to_execute_;

  map<unsigned, map<string, vector<Double>>>  proportions_;
  map<unsigned, map<string, vector<Double>>>  error_values_;

};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_REMOVALS_BY_AGE_RETAINED_TOTAL_H_ */
