/**
 * @file MortalityEventBiomass.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 8/11/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef MORTALITYEVENTBIOMASS_H_
#define MORTALITYEVENTBIOMASS_H_

// headers
#include "Model/Model.h"
#include "Partition/Accessors/Categories.h"
#include "Penalties/Common/Process.h"
#include "Processes/Process.h"
#include "Utilities/Math.h"

// namespaces
namespace niwa {
class Selectivity;
namespace processes {
namespace age {
namespace accessor = niwa::partition::accessors;

/**
 * class definition
 */
class MortalityEventBiomass : public niwa::Process {
public:
  // methods
  explicit MortalityEventBiomass(shared_ptr<Model> model);
  virtual                     ~MortalityEventBiomass() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoExecute() override final;
  void                        FillReportCache(ostringstream& cache) override final;
  void                        FillTabularReportCache(ostringstream& cache, bool first_run) override final;

private:
  // members
  accessor::Categories        partition_;
  vector<string>              category_labels_;
  vector<string>              selectivity_labels_;
  vector<Selectivity*>        selectivities_;
  Double                      u_max_ = 0;
  vector<Double>              catches_;
  vector<Double>              exploitation_by_year_;
  vector<Double>              actual_catches_;
  map<unsigned, Double>       catch_years_;
  Double                      exploitation_ = 0;
  vector<unsigned>            years_;
  string                      penalty_label_ = "";
  penalties::Process*         penalty_ = nullptr;
  string                      unit_;

};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* MORTALITYEVENTBIOMASS_H_ */
