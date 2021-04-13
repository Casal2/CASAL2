/**
 * @file InitialisationPartitionMeanWeight.h
 * @author C.Marsh
 * @date17/07/2017
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print the initial partition mean weight. Can be useful when investigating the initial state
 */
#ifndef LENGTH_REPORTS_CHILDREN_INITIALISATIONPARTITION_MEANWEIGHT_H_
#define LENGTH_REPORTS_CHILDREN_INITIALISATIONPARTITION_MEANWEIGHT_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {
namespace length {

// classes
class InitialisationPartitionMeanWeight : public niwa::Report {
public:
  InitialisationPartitionMeanWeight();
  virtual                     ~InitialisationPartitionMeanWeight() = default;
  void                        DoValidate(shared_ptr<Model> model) final { };
  void                        DoBuild(shared_ptr<Model> model) final { };
  void                        DoExecute(shared_ptr<Model> model) final;
  void                        DoExecuteTabular(shared_ptr<Model> model) final { };
};

} /* namespace length */
} /* namespace reports */
} /* namespace niwa */

#endif /* LENGTH_REPORTS_CHILDREN_INITIALISATIONPARTITION_MEANWEIGHT_H_ */
