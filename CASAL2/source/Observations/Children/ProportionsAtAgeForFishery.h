/**
 * @file ProportionsAtAgeForFishery.h
 * @author  C Marsh
 * @version 1.0
 * @date 25/08/15
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 */
#ifndef OBSERVATIONS_PROPORTIONSATAGEFORFISHERY_H_
#define OBSERVATIONS_PROPORTIONSATAGEFORFISHERY_H_

// Headers
#include "Observations/Observation.h"

#include "Partition/Accessors/CombinedCategories.h"
#include "Processes/Children/MortalityInstantaneous.h"
#include "AgeingErrors/AgeingError.h"

// Namespace
namespace niwa {
namespace observations {

using partition::accessors::CombinedCategoriesPtr;
using processes::MortalityInstantaneous;

/**
 * Class Definition
 */
class ProportionsAtAgeForFishery : public niwa::Observation {
public:
  // Methods
  explicit ProportionsAtAgeForFishery(Model* model);
  virtual                     ~ProportionsAtAgeForFishery();
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
  Double                        delta_ = 0.0;
  parameters::Table*            obs_table_ = nullptr;
  Double                        tolerance_ = 0.0;
  vector<Double>                process_error_values_;
  map<unsigned, Double>         process_errors_by_year_;
  string                        ageing_error_label_ = "";
  vector<string>                fishery_;
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
#endif /* OBSERVATIONS_PROPORTIONSATAGEFORFISHERY_H_ */
