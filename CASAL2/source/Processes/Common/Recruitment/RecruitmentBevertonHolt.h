/**
 * @file RecruitmentBevertonHolt.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 12/07/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This class is the Beverton-Holt recruitment process, consolidated for both
 * Age and Length partitioned models. It also implements the Ricker recruitment
 * process (type=recruitment_ricker, Age only), selected by the SRCurve constructor
 * argument: the two processes differ only in the stock-recruit curve (CalculateSR).
 *
 * Ricker previously had its own class. Folding it in normalised these behaviours
 * onto the Beverton-Holt ones:
 *  - steepness is bounded [0.2, 1.0] via Validate() rather than set_range() (same bounds)
 *  - recruitment_multipliers may be given as a single value and expanded to all years
 *  - projection (-f) requires an @project on recruitment_multipliers or
 *    standardised_recruitment_multipliers, and projecting the standardised values is supported
 *  - standardised_recruitment_multipliers is addressable for kAll (was kLookup)
 *  - report cache prints ssb_offset before recruit_event_SSB_percent
 * Deliberately NOT normalised: when standardise_years is omitted (and no simplex is
 * detected), Ricker does no standardisation, whereas Beverton-Holt defaults
 * standardise_years to all model years.
 */
#ifndef PROCESSES_COMMON_RECRUITMENTBEVERTONHOLT_H_
#define PROCESSES_COMMON_RECRUITMENTBEVERTONHOLT_H_

// headers
#include "Processes/Common/Recruitment/RecruitmentStockRecruit.h"

// namespaces
namespace niwa::processes::common {

/**
 * class definition
 */
class RecruitmentBevertonHolt : public RecruitmentStockRecruit {
public:
  // stock-recruit curve variants
  enum class SRCurve { kBevertonHolt, kRicker };

  // methods
  RecruitmentBevertonHolt(shared_ptr<Model> model, SRCurve sr_curve = SRCurve::kBevertonHolt);
  virtual ~RecruitmentBevertonHolt() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoVerify(shared_ptr<Model> model) override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;
  void RebuildCache() override final { DoReset(); }

private:
  // hooks
  Double CalculateSR(Double ssb_ratio) override final;
  Double CalculateYCS(unsigned current_year) override final;
  void   DistributeRecruits(Double amount_per) override final;
  string curve_name() const;

  // members
  const SRCurve              sr_curve_;
  vector<unsigned>           standardise_years_;
  vector<Double>             recruitment_multipliers_;
  map<unsigned, Double>      recruitment_multipliers_by_year_;
  map<unsigned, Double>      standardised_recruitment_multipliers_by_year_;
  bool                       standardise_recruitment_multipliers_ = true;
  bool                       simplex_standardise_detected_        = false;
  Double                     mean_ycs_                            = 1.0;

  // Length-specific members
  vector<Double>         initial_mean_length_;
  vector<Double>         initial_length_cv_;
  vector<vector<Double>> initial_length_distribution_;  // category x length bins

  // deprecated
  vector<unsigned> ycs_years_;
  vector<unsigned> standardise_ycs_years_;
  vector<Double>   ycs_values_;
  bool             project_standardised_ycs_ = false;
};

} /* namespace niwa::processes::common */

#endif /* PROCESSES_COMMON_RECRUITMENTBEVERTONHOLT_H_ */
