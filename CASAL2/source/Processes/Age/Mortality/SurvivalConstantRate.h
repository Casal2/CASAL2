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
#include "Processes/Common/Mortality/MortalityRateBase.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa {
namespace processes {
namespace age {
using utilities::OrderedMap;
/**
 * Class Definition
 */
class SurvivalConstantRate : public niwa::processes::common::MortalityRateBase {
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
  vector<Double>             s_input_;
  OrderedMap<string, Double> s_;
  vector<double>             ratios_;
  map<unsigned, double>      time_step_ratios_;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* SURVIVALCONSTANTRATE_H_ */
