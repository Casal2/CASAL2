/**
 * @file MortalityDiseaseRate.h
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This class is a replicates CASAL's disease mortality class.
 *
 */
#ifndef COMMON_MORTALITYDISEASERATE_H_
#define COMMON_MORTALITYDISEASERATE_H_

// Headers
#include "../../../Utilities/Types.h"
#include "MortalityRateBase.h"

// namespaces
namespace niwa::processes::common {
using utilities::OrderedMap;
/**
 * Class Definition
 */
class MortalityDiseaseRate : public niwa::processes::common::MortalityRateBase {
public:
  // Methods
  MortalityDiseaseRate(shared_ptr<Model> model);
  virtual ~MortalityDiseaseRate() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;

  const vector<string>& category_labels() const { return category_labels_; }

private:
  // Members
  vector<Double>                 dm_input_;
  OrderedMap<string, Double>     dm_;
  map<unsigned, Double>          year_effect_by_year_;
  vector<Double>                 year_effect_input_;
  vector<unsigned>               process_years_;
  vector<vector<vector<Double>>> results_;  // year x category x age classes
};

}  // namespace niwa::processes::common
#endif /* COMMON_MORTALITYDISEASERATE_H_ */
