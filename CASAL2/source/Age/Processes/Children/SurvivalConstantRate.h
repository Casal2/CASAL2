/**
 * @file SurvivalConstantRate.h
 * @author  C.Marsh
 * @version 1.0
 * @date 17/07/16
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a constant annual survival rate class. Every year
 * this process is executed individuals will survive at a constant rate.
 *
 */
#ifndef SURVIVALCONSTANTRATE_H_
#define SURVIVALCONSTANTRATE_H_

// Headers
#include "Common/Partition/Accessors/Categories.h"
#include "Common/Processes/Process.h"
#include "Common/Utilities/Types.h"

// namespaces
namespace niwa {
class Selectivity; // Forward declare Selectivity class
namespace age {
namespace processes {
namespace accessor = niwa::partition::accessors;
using utilities::OrderedMap;
/**
 * Class Definition
 */
class SurvivalConstantRate : public niwa::Process {
public:
  // Methods
  SurvivalConstantRate(Model* model);
  virtual                     ~SurvivalConstantRate() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoExecute() override final;
  void                        FillReportCache(ostringstream& cache) override final { };
  void                        FillTabularReportCache(ostringstream& cache, bool first_run) override final { };
private:
  // Members
  vector<string>              category_labels_;
  vector<Double>              s_input_;
  OrderedMap<string, Double>  s_;
  vector<Double>              ratios_;
  map<unsigned, Double>       time_step_ratios_;
  vector<vector<Double>>      survival_rates_;
  vector<string>              selectivity_names_;
  accessor::Categories        partition_;
  vector<Selectivity*>        selectivities_;
};

} /* namespace processes */
} /* namespace age */
} /* namespace niwa */
#endif /* SURVIVALCONSTANTRATE_H_ */
