/**
 * @file SurvivalConstantRate.h
 * @author  C.Marsh
 * @version 1.0
 * @date 17/07/16
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
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
#include "Partition/Accessors/Categories.h"
#include "Processes/Process.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {
class Selectivity;  // Forward declare Selectivity class
namespace processes {
namespace age {
namespace accessor = niwa::partition::accessors;
using utilities::OrderedMap;
/**
 * Class Definition
 */
class SurvivalConstantRate : public niwa::Process {
public:
  // Methods
  SurvivalConstantRate(shared_ptr<Model> model);
  virtual ~SurvivalConstantRate() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final{};
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final{};

private:
  // Members
  vector<string>             category_labels_;
  vector<Double>             s_input_;
  OrderedMap<string, Double> s_;
  vector<double>             ratios_;
  map<unsigned, double>      time_step_ratios_;
  vector<vector<Double>>     survival_rates_;
  vector<string>             selectivity_names_;
  accessor::Categories       partition_;
  vector<Selectivity*>       selectivities_;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* SURVIVALCONSTANTRATE_H_ */
