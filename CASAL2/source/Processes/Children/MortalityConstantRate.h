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


// namespaces
namespace niwa {
class Selectivity;

namespace processes {
namespace accessor = niwa::partition::accessors;

/**
 * Class Definition
 */
class MortalityConstantRate : public niwa::Process {
public:
  // Methods
  MortalityConstantRate(Model* model);
  virtual                     ~MortalityConstantRate() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoExecute() override final;

private:
  // Members
  vector<string>              category_labels_;
  vector<Double>              m_;
  vector<Double>              ratios_;
  map<unsigned, Double>       time_step_ratios_;
  vector<vector<Double>>      mortality_rates_;
  vector<string>              selectivity_names_;
  accessor::Categories        partition_;
  vector<Selectivity*>        selectivities_;
};

} /* namespace processes */
} /* namespace niwa */
#endif /* MORTALITYCONSTANTRATE_H_ */
