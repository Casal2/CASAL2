/**
 * @file MortalityInitialisationEventRemoval.h
 * @author  C.Marsh
 * @date 6/4/2017
 * @section LICENSE
 *
 * Copyright Casal2 Project 2026 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * An initialisation-phase mortality process. Every iteration of the initialisation phase in which
 * this process is executed, fish are killed off by an "event": the user supplies a catch value,
 * and the amount removed is capped by u_max (with an optional penalty if the requested catch
 * cannot be taken). It serves two user-facing process types -- mortality_initialisation_event (a
 * numbers-at-age catch, RemovalUnit::kNumbers) and mortality_initialisation_event_biomass (a
 * biomass catch, RemovalUnit::kBiomass, weighted by mean-weight-at-age when working out the
 * exploitation rate) -- which share every line of code except how the vulnerable stock is summed
 * (see DoExecute()). Factory.cpp picks which unit an instance uses via the constructor argument;
 * everything else about the two process types is identical.
 *
 * This is the initialisation-phase counterpart of MortalityEventRemoval, and reuses that class's
 * RemovalUnit enum rather than declaring a second one with the same meaning.
 *
 * Consolidated from the formerly-separate MortalityInitialisationEvent and
 * MortalityInitialisationEventBiomass classes. Both previously carried an `init_iteration_`
 * counter that was incremented but never read anywhere -- its only apparent readers were inside
 * commented-out StoreForReport() blocks, and StoreForReport() no longer exists as an API -- so it
 * has been dropped along with those dead comment blocks. The two classes also incremented it at
 * different places (inside vs outside the State::kInitialise guard), a divergence that was
 * unobservable precisely because nothing read the member.
 *
 * Merging surfaced one genuine behavioural asymmetry, normalized here per an explicit
 * consolidation decision: mortality_initialisation_event rejected a u_max of exactly 0.0 or 1.0
 * and mortality_initialisation_event_biomass accepted it. Both now accept it (see DoValidate()).
 * That is the only configuration for which this class's behaviour differs from the pre-merge
 * classes; for every configuration that validated before, the numeric output is unchanged.
 */
#ifndef PROCESSES_AGE_MORTALITYINITIALISATIONEVENTREMOVAL_H_
#define PROCESSES_AGE_MORTALITYINITIALISATIONEVENTREMOVAL_H_

// Headers
#include "Partition/Accessors/Categories.h"
#include "Penalties/Common/Process.h"
#include "Processes/Age/Mortality/MortalityEventRemoval.h"
#include "Processes/Age/Mortality/MortalityFisheryAccessorStub.h"
#include "Selectivities/Selectivity.h"

// namespaces
namespace niwa::processes::age {
namespace accessor = niwa::partition::accessors;

/**
 * Class Definition
 */
class MortalityInitialisationEventRemoval : public niwa::processes::age::MortalityFisheryAccessorStub {
public:
  // Methods
  MortalityInitialisationEventRemoval(shared_ptr<Model> model, RemovalUnit unit);
  virtual ~MortalityInitialisationEventRemoval() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final {};
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final {};
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final {};

private:
  // Members
  RemovalUnit                        unit_;
  Double                             catch_;
  double                             u_max_;
  vector<string>                     selectivity_names_;
  vector<Selectivity*>               selectivities_;
  string                             penalty_name_;
  penalties::Process*                penalty_ = nullptr;
  accessor::Categories               partition_;
  map<string, map<unsigned, Double>> vulnerable_;
};

}  // namespace niwa::processes::age
#endif /* PROCESSES_AGE_MORTALITYINITIALISATIONEVENTREMOVAL_H_ */
