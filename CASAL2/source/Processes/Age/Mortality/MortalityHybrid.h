/**
 * @file MortalityHybrid.h
 * @author C Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This mortality process is taken from Stock Synthesis see the hybrid approach
 * found at
 * https://github.com/nmfs-stock-synthesis/stock-synthesis/blob/main/SS_popdyn.tpl
 */
#ifndef SOURCE_PROCESSES_CHILDREN_MORTALITYHBRID_H_
#define SOURCE_PROCESSES_CHILDREN_MORTALITYHBRID_H_

// headers
#include "Processes/Common/Mortality/MortalityFisheryBase.h"

// namespaces
namespace niwa {
namespace processes {
namespace age {

// classes
class MortalityHybrid : public common::MortalityFisheryBase {
public:
  // methods
  explicit MortalityHybrid(shared_ptr<Model> model);
  virtual ~MortalityHybrid() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;

protected:
  void DoVerify(shared_ptr<Model> model) override final;

  // Method table columns specific to this process
  void ParseFisheryColumns(const MethodRow& row, FisheryData& fishery) override final;
  void CheckRepeatedFishery(const MethodRow& row, const FisheryData& stored, const FisheryData& repeat, unsigned first_row) override final;

private:
  bool           is_catch_biomass_ = true;
  unsigned       F_iterations_;
  Double         max_F_;
  vector<Double> annual_duration_by_timestep_;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_MORTALITYHBRID_H_ */
