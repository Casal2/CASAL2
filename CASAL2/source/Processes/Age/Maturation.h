/**
 * @file Maturation.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 11/09/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef PROCESSES_MATURATION_H_
#define PROCESSES_MATURATION_H_

// headers
#include "Processes/Process.h"
#include "Partition/Accessors/Categories.h"

// namespaces
namespace niwa {
class Selectivity;
namespace processes {
namespace age {
namespace accessor = niwa::partition::accessors;

/**
 *
 */
class Maturation : public niwa::Process {
public:
  // methods
  explicit Maturation(shared_ptr<Model> model);
  virtual                     ~Maturation() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        DoExecute() override final;
  void                        FillReportCache(ostringstream& cache) override final;
  void                        FillTabularReportCache(ostringstream& cache, bool first_run) override final;

protected:
  // Members
  vector<string>              from_category_names_;
  vector<string>              to_category_names_;
  vector<Double>              rates_;
  vector<unsigned>            years_;
  vector<string>              selectivity_names_;
  accessor::Categories        from_partition_;
  accessor::Categories        to_partition_;
  vector<Selectivity*>        selectivities_;
  map<unsigned, Double>       rates_by_years_;
};

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* MATURATION_H_ */
