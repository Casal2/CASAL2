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
#ifndef PROPORTIONSATAGE_H_
#define PROPORTIONSATAGE_H_

// Headers
#include "Observations/Observation.h"

#include "Partition/Accessors/CombinedCategories.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"

// Namespace
namespace niwa {
namespace observations {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;

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
  unsigned                      min_age_;
  unsigned                      max_age_;
  bool                          age_plus_;
  unsigned                      age_spread_;
  Double                        delta_;
  parameters::TablePtr          obs_table_;
  map<unsigned, vector<Double>> obs_by_year_;
  Double                        tolerance_;
  vector<Double>                process_errors_;
  map<unsigned, Double>         process_errors_by_year_;
  string                        ageing_error_label_;
  vector<Double>                error_values_;
  map<unsigned, Double>         error_values_by_year_;
  CachedCombinedCategoriesPtr   cached_partition_;
  CombinedCategoriesPtr         partition_;
  vector<Double>                age_results_;

  map<unsigned, map<string, vector<Double>>> proportions_;
};

} /* namespace observations */
} /* namespace niwa */
#endif /* PROPORTIONSATAGE_H_ */
