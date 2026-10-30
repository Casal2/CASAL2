/**
 * @file MortalityInstantaneousRetained.h
 * @author Samik Datta and Craig Marsh (samik.datta@niwa.co.nz)
 * @github https://github.com/Zaita
 * @date 26/06/2019
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This mortality process is a combination of the
 * constant and event mortality processes, with retained and discard mortality processes.
 */
#ifndef SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUSRETAINED_H_
#define SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUSRETAINED_H_

// headers
#include "Processes/Common/Mortality/MortalityFisheryBase.h"

// namespaces
namespace niwa {
namespace processes {
namespace age {

// classes
class MortalityInstantaneousRetained : public common::MortalityFisheryBase {
public:
  // methods
  explicit MortalityInstantaneousRetained(shared_ptr<Model> model);
  virtual ~MortalityInstantaneousRetained() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;
  // accessors
  map<unsigned, map<string, map<string, vector<Double>>>>& catch_at() { return removals_by_year_fishery_category_; };
  map<unsigned, map<string, map<string, vector<Double>>>>& retained_data() { return retained_by_year_fishery_category_; };

protected:
  // Method table columns specific to this process
  void ParseFisheryColumns(const MethodRow& row, FisheryData& fishery) override final;
  void CheckRepeatedFishery(const MethodRow& row, const FisheryData& stored, const FisheryData& repeat, unsigned first_row) override final;
  void InitialiseFishery(FisheryData& fishery) override final;
  void ParseFisheryCategoryColumns(const MethodRow& row, unsigned category_count, unsigned first_fishery_category) override final;

private:
  // members for observations
  map<unsigned, map<string, map<string, vector<Double>>>> removals_by_year_fishery_category_;           // Year,  fishery, category
  map<unsigned, map<string, map<string, vector<Double>>>> retained_by_year_fishery_category_;           // Year,  fishery, category
  map<unsigned, map<string, map<string, vector<Double>>>> discards_by_year_fishery_category_;           // Year,  fishery, category
  map<unsigned, map<string, map<string, vector<Double>>>> discard_mortality_by_year_fishery_category_;  // Year,  fishery, category

  map<unsigned, map<string, vector<string>>> year_method_category_to_store_;  // Year,  fishery, category

  // Members for reporting
  vector<vector<vector<Double>>> removals_by_year_category_age_;  // year[year_ndx][category_ndx][age_ndx]

  vector<vector<Double>> removals_by_category_age_;  // [category_ndx][age_ndx]
  vector<vector<Double>> retained_by_category_age_;  // [category_ndx][age_ndx]
  vector<vector<Double>> discards_by_category_age_;  // [category_ndx][age_ndx]
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_MORTALITYINSTANTANEOUSRETAINED_H_ */
