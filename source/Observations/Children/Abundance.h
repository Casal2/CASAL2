/**
 * @file Abundance.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/03/2013
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
#ifndef ABUNDANCE_H_
#define ABUNDANCE_H_

// Headers
#include "Observations/Observation.h"

#include "Catchabilities/Catchability.h"
#include "Partition/Accessors/CombinedCategories.h"
#include "Partition/Accessors/Cached/CombinedCategories.h"

// Namespaces
namespace isam {
namespace observations {

using partition::accessors::CombinedCategoriesPtr;
using partition::accessors::cached::CachedCombinedCategoriesPtr;

/**
 * Class definition
 */
class Abundance : public isam::Observation {
public:
  // Methods
  Abundance();
  virtual                     ~Abundance() = default;
  void                        Validate() override final;
  void                        Build() override final;
  void                        PreExecute() override final;
  void                        Execute() override final;

private:
  // Members
  vector<Double>              proportions_;
  vector<Double>              error_values_;
  string                      catchability_label_;
  CatchabilityPtr             catchability_;
  Double                      delta_;
  Double                      process_error_;
  CachedCombinedCategoriesPtr cached_partition_;
  CombinedCategoriesPtr       partition_;

};

} /* namespace observation */
} /* namespace isam */
#endif /* ABUNDANCE_H_ */
