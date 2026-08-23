/**
 * @file MortalityConstantRemovalRate.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 20/12/2012
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This class is a constant annual removal-rate process. Every year this process is executed
 * fish will be killed off at a constant rate. It serves two user-facing process types --
 * mortality_constant_rate (an instantaneous-rate/exponential-decay formulation, m) and
 * mortality_constant_exploitation (a direct exploitation-rate formulation, u) -- which share
 * every line of code except the bound parameter name/description and the one-line formula
 * (RemovalFormulation, see MortalityRateFormulas.h). Factory.cpp picks which formulation an
 * instance uses via the constructor argument; everything else about the two process types is
 * identical.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PROCESSES_COMMON_MORTALITYCONSTANTREMOVALRATE_H_
#define PROCESSES_COMMON_MORTALITYCONSTANTREMOVALRATE_H_

// Headers
#include "Partition/Accessors/Categories.h"
#include "Processes/Age/Mortality/MortalityFisheryAccessorStub.h"
#include "Processes/Common/Mortality/MortalityRateBase.h"
#include "Processes/Common/Mortality/MortalityRateFormulas.h"
#include "Processes/Process.h"
#include "Utilities/Types.h"

// namespaces
namespace niwa::processes::common {
namespace accessor = niwa::partition::accessors;
using utilities::OrderedMap;
/**
 * Class Definition
 */
class MortalityConstantRemovalRate : public niwa::processes::age::MortalityFisheryAccessorStub {
public:
  // Methods
  MortalityConstantRemovalRate(shared_ptr<Model> model, RemovalFormulation formulation);
  virtual ~MortalityConstantRemovalRate() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;

private:
  // Members
  RemovalFormulation             formulation_;
  const RemovalFormulationSpec&  spec_;
  vector<Double>                 m_input_;
  OrderedMap<string, Double>     m_;
  vector<double>                 ratios_;
  map<unsigned, double>          time_step_ratios_;
  vector<string>                 selectivity_names_;
  accessor::Categories           partition_;
  vector<niwa::Selectivity*>     selectivities_;
  vector<Double>                 total_removals_by_year_;
};

}  // namespace niwa::processes::common
#endif /* PROCESSES_COMMON_MORTALITYCONSTANTREMOVALRATE_H_ */
