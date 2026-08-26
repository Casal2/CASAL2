/**
 * @file RecruitmentBevertonHoltWithDeviations.h
 * @author C.Marsh
 * @date 11/7/17
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This class is the Beverton-Holt recruitment process, that is parameterised with deviations instead of raw YCSs as is done in CASAL
 */
#ifndef PROCESSES_COMMON_RECRUITMENTBEVERTONHOLT_WITHDEVIATIONS_H_
#define PROCESSES_COMMON_RECRUITMENTBEVERTONHOLT_WITHDEVIATIONS_H_

// headers
#include "Estimates/Estimate.h"
#include "Processes/Common/Recruitment/RecruitmentStockRecruit.h"

// namespaces
namespace niwa::processes::common {

/**
 * class definition
 */
class RecruitmentBevertonHoltWithDeviations : public RecruitmentStockRecruit {
public:
  // methods
  RecruitmentBevertonHoltWithDeviations(shared_ptr<Model> model);
  virtual ~RecruitmentBevertonHoltWithDeviations() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;

private:
  // hooks
  Double CalculateSR(Double ssb_ratio) override final;
  Double CalculateYCS(unsigned current_year) override final;

  // members
  Double                b_max_;
  map<unsigned, Double> bias_by_year_;
  unsigned               year1_;
  unsigned               year2_;
  unsigned               year3_;
  unsigned               year4_;
  Double                 sigma_r_;
  vector<Double>         recruit_dev_values_;
  map<unsigned, Double>  recruit_dev_value_by_year_;
  vector<Double>         ycs_values_;

  // deprecated
  vector<unsigned> recruit_dev_years_;
};

} /* namespace niwa::processes::common */

#endif /* PROCESSES_COMMON_RECRUITMENTBEVERTONHOLT_WITHDEVIATIONS_H_ */
