/**
 * @file RecruitmentStockRecruit.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 26/08/2026
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * Intermediate base class for the stock-recruit family of recruitment processes
 * (Beverton-Holt, Ricker, Beverton-Holt-with-deviations). It owns the logic that is
 * common to all of them: SSB lookup (including the initialisation-phase fallback),
 * ssb_ratio computation, ScalePartition(), category-proportion recruit distribution,
 * and the shared parts of validation/build/reset/report-cache bookkeeping. Each
 * concrete child supplies the one-line stock-recruit curve (CalculateSR) and its own
 * YCS-input mechanism (CalculateYCS) as the two things that actually differ between
 * these processes.
 */
#ifndef PROCESSES_COMMON_RECRUITMENTSTOCKRECRUIT_H_
#define PROCESSES_COMMON_RECRUITMENTSTOCKRECRUIT_H_

// headers
#include "DerivedQuantities/DerivedQuantity.h"
#include "Partition/Accessors/Categories.h"
#include "Processes/Process.h"

// namespaces
namespace niwa::processes::common {

namespace accessor = niwa::partition::accessors;
using utilities::OrderedMap;

/**
 * class definition
 */
class RecruitmentStockRecruit : public niwa::Process {
public:
  // methods
  explicit RecruitmentStockRecruit(shared_ptr<Model> model);
  virtual ~RecruitmentStockRecruit() = default;

  void DoExecute() override final;

  void ScalePartition();
  bool has_partition_been_scaled() { return have_scaled_partition; };

  // accessor
  unsigned              ssb_offset() { return ssb_offset_; };
  string                ssb_label() { return ssb_label_; };
  bool                  b0_initialised() { return b0_initialised_; };
  const vector<string>& category_labels() const { return category_labels_; }

protected:
  // hooks for the stock-recruit equation and YCS-input mechanism
  virtual Double CalculateSR(Double ssb_ratio)          = 0;
  virtual Double CalculateYCS(unsigned current_year)    = 0;
  virtual void   DistributeRecruits(Double amount_per);

  // shared helper methods, called by each concrete child's own DoValidate/DoBuild/DoReset/FillReportCache
  void ValidateCategoriesProportionsAndCore();
  void ValidateAndWarnAge();
  void ResolveB0AndDerivedQuantity();
  void ComputeTimeStepIndices(unsigned& ageing_index, unsigned& derived_quantity_index, unsigned& recruitment_index, unsigned& ageing_processes);
  void BuildSpawnEventYears();
  void ValidateR0B0NotBothEstimated();
  void ResizeYearlyCaches();
  void ResetPartitionAndProportions();
  void AppendSpawnEventYearField(ostringstream& cache) const;
  void AppendCoreReportFields(ostringstream& cache) const;
  void FillMultiplierTabularReportCache(ostringstream& cache, bool first_run, const map<unsigned, Double>& standardised_recruitment_multipliers_by_year,
                                         const map<unsigned, Double>& recruitment_multipliers_by_year) const;

  // members
  accessor::Categories       partition_;
  vector<string>             category_labels_;
  Double                     r0_ = 0.0;
  string                     phase_b0_label_;
  unsigned                   phase_b0_ = 0;
  Double                     b0_             = 0.0;
  bool                       b0_initialised_ = false;
  vector<Double>             proportions_;
  unsigned                   age_       = 0;
  Double                     steepness_ = 0.0;
  string                     ssb_label_;
  unsigned                   ssb_offset_ = 0;
  vector<Double>             ssb_values_;
  vector<unsigned>           spawn_event_years_;
  vector<Double>             true_ycs_values_;
  vector<Double>             recruitment_values_;
  bool                       have_scaled_partition = false;
  DerivedQuantity*           derived_quantity_     = nullptr;
  unsigned                   year_counter_         = 0;
  OrderedMap<string, Double> proportions_by_category_;
  vector<unsigned>           years_;
};

} /* namespace niwa::processes::common */

#endif /* PROCESSES_COMMON_RECRUITMENTSTOCKRECRUIT_H_ */
