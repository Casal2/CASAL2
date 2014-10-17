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
namespace isam {
namespace observations {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;

/**
 * Class Definition
 */
class ProportionsAtAge : public isam::Observation {
public:
  // Methods
  ProportionsAtAge() = default;
  virtual                     ~ProportionsAtAge() = default;
  void                        DoValidate() override final;
  virtual void                DoBuild() override;
  void                        DoReset() override final { };
  void                        PreExecute() override final;
  void                        Execute() override final;
  void                        CalculateScore() override final;
  bool                        HasYear(unsigned year) const override final { return year == year_; }

protected:
  // Members
  unsigned                    year_;
  unsigned                    min_age_;
  unsigned                    max_age_;
  bool                        age_plus_;
  unsigned                    age_spread_;
  Double                      delta_;
  vector<string>              obs_;
  Double                      tolerance_;
  Double                      process_error_;
  string                      ageing_error_label_;
  vector<vector<Double> >     proportions_;
  vector<Double>              error_values_;
  vector<vector<Double> >     error_values_by_category_;
  CachedCombinedCategoriesPtr cached_partition_;
  CombinedCategoriesPtr       partition_;
  vector<Double>              age_results_;
};

} /* namespace observations */
} /* namespace isam */
#endif /* PROPORTIONSATAGE_H_ */
