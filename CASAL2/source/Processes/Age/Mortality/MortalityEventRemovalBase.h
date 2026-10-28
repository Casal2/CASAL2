/**
 * @file MortalityEventRemovalBase.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/10/2026
 * @section LICENSE
 *
 * Copyright Casal2 Project 2026 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * Shared removal core for the two catch-driven "event" mortality processes:
 * MortalityEventRemoval (mortality_event, mortality_event_biomass) and
 * MortalityInitialisationEventRemoval (mortality_initialisation_event,
 * mortality_initialisation_event_biomass).
 *
 * Both classes take a catch, sum the vulnerable stock (as numbers or biomass, see RemovalUnit),
 * work out exploitation = catch / ZeroFun(vulnerable), cap it at u_max (triggering the optional
 * penalty), and remove cached_vulnerable * exploitation from each cell. That core lives here as
 * protected helpers; each child keeps what genuinely differs -- where the catch comes from (a
 * per-year map vs a scalar), which model state it runs in (outside vs only during
 * initialisation), its handling of a negative exploitation, and its reporting.
 *
 * This is deliberately not a full class merge: a phase flag plus optional reporting would turn
 * the two children into one leaky abstraction (see updated_merge_details.md TBM-3).
 *
 * Parameter binding stays in each child so bind order and descriptions are unchanged; the
 * members they bind into are declared here.
 */
#ifndef PROCESSES_AGE_MORTALITYEVENTREMOVALBASE_H_
#define PROCESSES_AGE_MORTALITYEVENTREMOVALBASE_H_

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
 *
 * Shared by MortalityEventRemoval and MortalityInitialisationEventRemoval -- the two classes make
 * the same numbers-vs-biomass distinction, so they share one enum rather than declaring two with
 * identical meaning.
 */
enum class RemovalUnit {
  // vulnerable stock summed as numbers-at-age
  //   MortalityEventRemoval (mortality_event)
  //   MortalityInitialisationEventRemoval (mortality_initialisation_event)
  kNumbers,
  // vulnerable stock weighted by mean-weight-at-age
  //   MortalityEventRemoval (mortality_event_biomass)
  //   MortalityInitialisationEventRemoval (mortality_initialisation_event_biomass)
  kBiomass,
};

/**
 * Class Definition
 */
class MortalityEventRemovalBase : public niwa::processes::age::MortalityFisheryAccessorStub {
public:
  virtual ~MortalityEventRemovalBase() = default;

protected:
  /**
   * Result of CalculateExploitation(). actual_catch is the catch that can actually be taken:
   * vulnerable * u_max when capped, otherwise the requested catch.
   */
  struct CatchExploitation {
    Double exploitation = 0.0;
    Double actual_catch = 0.0;
  };

  // Methods
  MortalityEventRemovalBase(shared_ptr<Model> model, const string& label, RemovalUnit unit);
  void              BuildPartitionSelectivitiesAndPenalty();
  CatchExploitation CalculateExploitation(Double catch_amount);
  void              RemoveAtExploitation(Double exploitation);

  // Members
  RemovalUnit                        unit_;
  double                             u_max_ = 0;
  vector<string>                     selectivity_labels_;
  vector<Selectivity*>               selectivities_;
  string                             penalty_label_ = "";
  penalties::Process*                penalty_       = nullptr;
  accessor::Categories               partition_;
  map<string, map<unsigned, Double>> vulnerable_;
};

}  // namespace niwa::processes::age
#endif /* PROCESSES_AGE_MORTALITYEVENTREMOVALBASE_H_ */
