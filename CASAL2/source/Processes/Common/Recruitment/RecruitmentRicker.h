/**
 * @file RecruitmentRicker.h
 * @author  A Dunn
 * @date June 2023
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This class is the Ricker recruitment process
 */
#ifndef PROCESSES_COMMON_RECRUITMENTRICKER_H_
#define PROCESSES_COMMON_RECRUITMENTRICKER_H_

// headers
#include "Processes/Common/Recruitment/RecruitmentStockRecruit.h"

// namespaces
namespace niwa::processes::common {

/**
 * class definition
 */
class RecruitmentRicker : public RecruitmentStockRecruit {
public:
  // methods
  RecruitmentRicker(shared_ptr<Model> model);
  virtual ~RecruitmentRicker() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoVerify(shared_ptr<Model> model) override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;

private:
  // hooks
  Double CalculateSR(Double ssb_ratio) override final;
  Double CalculateYCS(unsigned current_year) override final;

  // members
  vector<unsigned>           standardise_years_;
  vector<Double>             recruitment_multipliers_;
  map<unsigned, Double>      recruitment_multipliers_by_year_;
  map<unsigned, Double>      standardised_recruitment_multipliers_by_year_;
  bool                       standardise_recruitment_multipliers_ = true;
  bool                       simplex_standardise_detected_        = false;

  // deprecated
  vector<unsigned> ycs_years_;
  vector<unsigned> standardise_ycs_years_;
  vector<Double>   ycs_values_;
};

} /* namespace niwa::processes::common */

#endif /* PROCESSES_COMMON_RECRUITMENTRICKER_H_ */
