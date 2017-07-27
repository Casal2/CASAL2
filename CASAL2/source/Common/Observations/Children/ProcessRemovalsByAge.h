/**
 * @file ProcessRemovalsByAge.h
 * @author  C Marsh
 * @version 1.0
 * @date 25/08/15
 * @section LICENSE
 *
 * Copyright NIWA Science 2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This observation is a specific process observation class. It is associated with the process type mortality_instantaneous.
 * It calls a catch at object that is created from the process which represents the numbers at age halfway trough the mortality process
 * This class then applies ageing error and converts to a proportion which then gets sent to a likelihood for evaluation.
 *
 */
#ifndef OBSERVATIONS_REMOVEALS_BY_AGE_H_
#define OBSERVATIONS_REMOVEALS_BY_AGE_H_

// Headers
#include "Common/Observations/Observation.h"

#include "Common/Partition/Accessors/CombinedCategories.h"
#include "Common/Processes/Children/MortalityInstantaneous.h"
#include "Age/AgeingErrors/AgeingError.h"

// Namespace
namespace niwa {
namespace observations {

using partition::accessors::CombinedCategoriesPtr;
using processes::MortalityInstantaneous;

/**
 * Class Definition
 */
class ProcessRemovalsByAge : public niwa::Observation {
public:
  // Methods
  explicit ProcessRemovalsByAge(Model* model);
  virtual                     ~ProcessRemovalsByAge();
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
  bool                          age_plus_ = false;
  unsigned                      age_spread_ = 0;
  parameters::Table*            obs_table_ = nullptr;
  Double                        tolerance_ = 0.0;
  vector<Double>                process_error_values_;
  map<unsigned, Double>         process_errors_by_year_;
  string                        ageing_error_label_ = "";
  vector<string>                method_;
  parameters::Table*            error_values_table_ = nullptr;
  CombinedCategoriesPtr         partition_;
  AgeingError*                  ageing_error_ = nullptr;
  vector<Double>                age_results_;
  MortalityInstantaneous*       mortality_instantaneous_ = nullptr;
  vector<string>                time_step_label_;
  string                        process_label_;

  map<unsigned, map<string, vector<Double>>>  proportions_;
  map<unsigned, map<string, vector<Double>>>  error_values_;

};

} /* namespace observations */
} /* namespace niwa */
#endif /* OBSERVATIONS_REMOVEALS_BY_AGE_H_ */
