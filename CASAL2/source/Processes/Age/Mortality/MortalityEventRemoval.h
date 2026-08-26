/**
 * @file MortalityEventRemoval.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 21/12/2012
 * @section LICENSE
 *
 * Copyright Casal2 Project 2026 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * A time-varying mortality process. Every year fish are killed off by an "event": the user
 * provides a catch value per year, and the amount removed is capped by u_max (with an optional
 * penalty if the requested catch cannot be taken). It serves two user-facing process types --
 * mortality_event (a numbers-at-age catch, RemovalUnit::kNumbers) and mortality_event_biomass (a
 * biomass catch, RemovalUnit::kBiomass, weighted by mean-weight-at-age when working out the
 * exploitation rate) -- which share every line of code except how the vulnerable stock is summed
 * (see DoExecute()). Factory.cpp picks which unit an instance uses via the constructor argument;
 * everything else about the two process types is identical.
 *
 * Consolidated from the formerly-separate MortalityEvent and MortalityEventBiomass classes.
 * MortalityEventBiomass previously carried a `unit_` (string) and a scalar `exploitation_` member
 * that were declared but never read or written anywhere -- dead code, almost certainly a vestige
 * of an earlier, abandoned attempt to give this class the numbers/biomass toggle it gets for real
 * here. Merging also surfaced (and, per an explicit consolidation decision, normalized) four small
 * behavioral asymmetries between the two original classes -- see the comments at each point of
 * normalization in MortalityEventRemoval.cpp -- so this class's numeric output for
 * mortality_event_biomass configurations can differ from the pre-merge class in the edge cases
 * those asymmetries covered (initialisation-phase removals, out-of-order years, u_max of exactly
 * 0 or 1). It is not a pure refactor.
 */
#ifndef PROCESSES_AGE_MORTALITYEVENTREMOVAL_H_
#define PROCESSES_AGE_MORTALITYEVENTREMOVAL_H_

// Headers
#include "Partition/Accessors/Categories.h"
#include "Penalties/Common/Process.h"
#include "Processes/Age/Mortality/MortalityFisheryAccessorStub.h"
#include "Selectivities/Selectivity.h"

// namespaces
namespace niwa::processes::age {
namespace accessor = niwa::partition::accessors;

/**
 * Which quantity the catch/u_max/vulnerable-stock calculation is expressed in.
 */
enum class RemovalUnit {
  kNumbers,  // MortalityEventRemoval (mortality_event) -- vulnerable stock summed as numbers-at-age
  kBiomass,  // MortalityEventRemoval (mortality_event_biomass) -- vulnerable stock weighted by mean-weight-at-age
};

/**
 * Class Definition
 */
class MortalityEventRemoval : public niwa::processes::age::MortalityFisheryAccessorStub {
public:
  // Methods
  MortalityEventRemoval(shared_ptr<Model> model, RemovalUnit unit);
  virtual ~MortalityEventRemoval() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;

private:
  // Members
  RemovalUnit                        unit_;
  vector<unsigned>                   years_;
  vector<Double>                     catches_;
  map<unsigned, Double>              catch_years_;
  double                             u_max_ = 0;
  vector<string>                     selectivity_labels_;
  vector<Selectivity*>               selectivities_;
  string                             penalty_label_ = "";
  penalties::Process*                penalty_       = nullptr;
  accessor::Categories                partition_;
  vector<Double>                      exploitation_by_year_;
  vector<Double>                     actual_catches_;
  map<string, map<unsigned, Double>> vulnerable_;
};

}  // namespace niwa::processes::age
#endif /* PROCESSES_AGE_MORTALITYEVENTREMOVAL_H_ */
