/**
 * @file CategoryTransitionByYear.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 11/09/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This process moves individuals from the from_category to the to_category in certain years based on a rate and selectivity
 */
#ifndef PROCESSES_CATEGORYTRANSITIONBYYEAR_H_
#define PROCESSES_CATEGORYTRANSITIONBYYEAR_H_

// headers
#include "Processes/Process.h"
#include "Partition/Accessors/Categories.h"

// namespaces
namespace niwa {
class Selectivity;

namespace processes {
namespace accessor = niwa::partition::accessors;

/**
 *
 */
class CategoryTransitionByYear : public niwa::Process {
public:
  // methods
  explicit CategoryTransitionByYear(Model* model);
  virtual                     ~CategoryTransitionByYear() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        DoExecute() override final;

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

} /* namespace processes */
} /* namespace niwa */
#endif /* PROCESSES_CATEGORYTRANSITIONBYYEAR_H_ */
