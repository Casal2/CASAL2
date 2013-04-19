/**
 * @file MortalityConstantRate.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a constant annual mortality rate class. Every year
 * this process is executed fish will be killed off at a constant rate.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef MORTALITYCONSTANTRATE_H_
#define MORTALITYCONSTANTRATE_H_

// Headers
#include "Partition/Accessors/Categories.h"
#include "Processes/Process.h"
#include "Selectivities/Selectivity.h"

// namespaces
namespace isam {
namespace processes {

namespace accessor = isam::partition::accessors;

/**
 * Class Definition
 */
class MortalityConstantRate : public isam::Process {
public:
  // Methods
  MortalityConstantRate();
  virtual                     ~MortalityConstantRate() = default;
  void                        Validate() override final;
  void                        Build() override final;
  void                        Execute() override final;
  void                        Reset() override final;

private:
  // Members
  vector<string>              category_names_;
  vector<Double>              m_;
  vector<vector<Double> >     mortality_rates_;
  vector<string>              selectivity_names_;
  accessor::CategoriesPtr     partition_;
  vector<SelectivityPtr>      selectivities_;
};

} /* namespace processes */
} /* namespace isam */
#endif /* MORTALITYCONSTANTRATE_H_ */
