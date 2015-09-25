/**
 * @file ProportionsAtAge.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 8/04/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef OBSERVATIONS_PROPORTIONSATAGE_H_
#define OBSERVATIONS_PROPORTIONSATAGE_H_

// Headers
#include "Observations/Observation.h"

#include "Partition/Accessors/CombinedCategories.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"
#include "Processes/Children/MortalityInstantaneous.h"
#include "AgeingErrors/AgeingError.h"

// Namespace
namespace niwa {
namespace observations {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;
using processes::MortalityInstantaneous;

/**
 * Class Definition
 */
class ProportionsAtAge : public niwa::Observation {
public:
  // Methods
  ProportionsAtAge();
  virtual                     ~ProportionsAtAge() = default;
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
  parameters::Table*            obs_table_;
  Double                        tolerance_ = 0.0;
  vector<Double>                process_error_values_;
  map<unsigned, Double>         process_errors_by_year_;
  string                        ageing_error_label_;
  parameters::Table*            error_values_table_;
  CachedCombinedCategoriesPtr   cached_partition_;
  CombinedCategoriesPtr         partition_;
  AgeingError*                  ageing_error_;
  vector<Double>                age_results_;
  MortalityInstantaneous*       mortality_instantaneous_;

  map<unsigned, map<string, vector<Double>>>  proportions_;
  map<unsigned, map<string, vector<Double>>>  error_values_;

};

} /* namespace observations */
} /* namespace niwa */
#endif /* PROPORTIONSATAGE_H_ */
