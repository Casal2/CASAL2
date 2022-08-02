/**
 * @file MortalityInitialisationBaranov.h
 * @author  C.Marsh
 * @version 1.0
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science �2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This is a mortality class that allows to apply an instaneous F, that is estimable with a selectivity for a set of categories
 *
 */
#ifndef MORTALITY_INITIALISATION_BARANOV_H_
#define MORTALITY_INITIALISATION_BARANOV_H_

// Headers
#include "Model/Model.h"
#include "Partition/Accessors/Categories.h"
#include "Processes/Process.h"
#include "Selectivities/Selectivity.h"
#include "Processes/Age/Mortality.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {

namespace accessor = niwa::partition::accessors;

/**
 * Class definition
 */
class MortalityInitialisationBaranov : public Mortality {
public:
  // Methods
  explicit MortalityInitialisationBaranov(shared_ptr<Model> model);
  virtual ~MortalityInitialisationBaranov() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoExecute() override final;
  void FillReportCache(ostringstream& cache) override final;
  void FillTabularReportCache(ostringstream& cache, bool first_run) override final;

private:
  // Members
  Double                              actual_catch_;

  Double                              f_;
  vector<string>                      selectivity_names_;
  accessor::Categories                partition_;
  vector<Selectivity*>                selectivities_;

};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* MORTALITY_INITIALISATION_BARANOV_H_ */
