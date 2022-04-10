/**
 * @file InitialisationPartition.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 25/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print the partition at the end of the initialisation
 * phases. It's bound to the kInitialise state of the model.
 */
#ifndef REPORTS_CHILDREN_INITIALISATIONPARTITION_H_
#define REPORTS_CHILDREN_INITIALISATIONPARTITION_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

// classes
class InitialisationPartition : public niwa::Report {
public:
  InitialisationPartition();
  virtual ~InitialisationPartition() = default;
  void DoValidate(shared_ptr<Model> model) final{};
  void DoBuild(shared_ptr<Model> model) final{};
  void DoExecute(shared_ptr<Model> model) final;

  void DoPrepareTabular(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final;
  void DoFinaliseTabular(shared_ptr<Model> model) final;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* AGE_REPORTS_CHILDREN_INITIALISATIONPARTITION_H_ */
