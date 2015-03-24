/**
 * @file StateCategoryByAge.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 24/03/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This process will input it's own values in to the partition
 * at a given state
 */
#ifndef SOURCE_PROCESSES_CHILDREN_STATECATEGORYBYAGE_H_
#define SOURCE_PROCESSES_CHILDREN_STATECATEGORYBYAGE_H_

// headers
#include "Partition/Accessors/Age/Categories.h"
#include "Processes/Process.h"

// namespaces
namespace niwa {
namespace processes {

namespace accessor = niwa::partition::accessors;

/**
 * Class definition
 */
class StateCategoryByAge : public niwa::Process {
public:
  StateCategoryByAge();
  virtual                     ~StateCategoryByAge() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final { };
  void                        DoExecute() override final;

private:
  // members
  accessor::Categories        partition_;
  vector<string>              category_labels_;
  unsigned                    min_age_;
  unsigned                    max_age_;
  parameters::TablePtr        n_table_;
  map<string, vector<Double>> n_;
};

} /* namespace processes */
} /* namespace niwa */

#endif /* SOURCE_PROCESSES_CHILDREN_STATECATEGORYBYAGE_H_ */
