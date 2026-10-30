/**
 * @file MortalityInstantaneous.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/07/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This mortality process is a combination of the constant and event mortality processes.
 * Consolidated implementation for both Age and Length partitioned models.
 */
#ifndef SOURCE_PROCESSES_COMMON_MORTALITYINSTANTANEOUS_H_
#define SOURCE_PROCESSES_COMMON_MORTALITYINSTANTANEOUS_H_

// headers
#include "MortalityFisheryBase.h"

// namespaces
namespace niwa::processes::common {

// classes
class MortalityInstantaneous : public MortalityFisheryBase {
public:
  // methods
  explicit MortalityInstantaneous(shared_ptr<Model> model);
  virtual ~MortalityInstantaneous() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;

  // Accessor methods for observations (shared across Age and Length)
  bool             check_categories_in_methods_for_removal_obs(vector<string> methods, vector<string> category_labels);
  bool             check_years_in_methods_for_removal_obs(vector<unsigned> years, vector<string> methods);
  bool             check_methods_for_removal_obs(vector<string> methods);
  vector<unsigned> get_fishery_ndx_for_catch_at(vector<string> fishery_labels);
  vector<unsigned> get_category_ndx_for_catch_at(vector<string> category_labels);
  vector<unsigned> get_year_ndx_for_catch_at(vector<unsigned> years);

protected:
  // Method table columns specific to this process
  void ParseFisheryColumns(const MethodRow& row, FisheryData& fishery) override final;
  void CheckRepeatedFishery(const MethodRow& row, const FisheryData& stored, const FisheryData& repeat, unsigned first_row) override final;
  void InitialiseFishery(FisheryData& fishery) override final;

private:
  bool is_catch_biomass_ = true;

  // Age-specific options
  bool use_catch_biomass_ = true;
  bool use_u_             = true;
};

}  // namespace niwa::processes::common

#endif /* SOURCE_PROCESSES_COMMON_MORTALITYINSTANTANEOUS_H_ */
