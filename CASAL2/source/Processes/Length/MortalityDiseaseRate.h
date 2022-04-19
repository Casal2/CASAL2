/**
 * @file MortalityDiseaseRate.h
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a replicates CASAL's disease mortality class.
 *
 */
#ifndef LENGTH_MORTALITYDISEASERATE_H_
#define LENGTH_MORTALITYDISEASERATE_H_

// Headers
#include "../../Partition/Accessors/Categories.h"
#include "../../Processes/Process.h"
#include "../../Utilities/Types.h"

// namespaces
namespace niwa {
// class Selectivity;
namespace processes {
namespace length {
namespace accessor = niwa::partition::accessors;
using utilities::OrderedMap;
/**
 * Class Definition
 */
class MortalityDiseaseRate : public niwa::Process {
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
  vector<string>              category_labels_;
  vector<Double>              dm_input_;
  OrderedMap<string, Double>  dm_;
  map<unsigned, Double>       year_effect_by_year_;
  vector<Double>              year_effect_input_;
  vector<string>              selectivity_names_;
  accessor::Categories partition_;
  vector<Selectivity*>        selectivities_;
  vector<unsigned>            process_years_;
  vector<vector<vector<Double>>>      results_; // year x category x length bin
};

} /* namespace length */
} /* namespace processes */
} /* namespace niwa */
#endif /* LENGTH_MORTALITYDISEASERATE_H_ */
