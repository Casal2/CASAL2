/**
 * @file TransitionCategoryByAge.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 10/12/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef PROCESSES_TRANSITIONCATEGORYBYAGE_H_
#define PROCESSES_TRANSITIONCATEGORYBYAGE_H_

// headers
#include "Common/Partition/Accessors/Categories.h"
#include "Common/Penalties/Penalty.h"
#include "Common/Processes/Process.h"
#include "Common/Selectivities/Selectivity.h"

// namespaces
namespace niwa {
namespace processes {

namespace accessor = niwa::partition::accessors;

/**
 * Class definition
 */
class TransitionCategoryByAge : public niwa::Process {
public:
  // methods
  explicit TransitionCategoryByAge(Model* model);
  virtual                     ~TransitionCategoryByAge();
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        DoExecute() override final;

private:
  // members
  vector<string>                from_category_labels_;
  vector<string>                to_category_labels_;
  vector<unsigned>              years_;
  unsigned                      min_age_;
  unsigned                      max_age_;
  string                        selectivity_label_;
  Selectivity*                  selectivity_ = nullptr;
  string                        penalty_label_;
  Penalty*                      penalty_ = nullptr;
  Double                        u_max_;
  parameters::Table*            n_table_ = nullptr;
  map<unsigned, vector<Double>> n_;
  accessor::Categories          to_partition_;
  accessor::Categories          from_partition_;
};

} /* namespace processes */
} /* namespace niwa */

#endif /* PROCESSES_TRANSITIONCATEGORYBYAGE_H_ */
