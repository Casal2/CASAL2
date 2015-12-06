/**
 * @file InitialisationPartition.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 25/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print the partition at the end of the initialisation
 * phases. It's bound to the kInitialise state of the model.
 */
#ifndef SOURCE_REPORTS_CHILDREN_INITIALISATIONPARTITION_H_
#define SOURCE_REPORTS_CHILDREN_INITIALISATIONPARTITION_H_

// headers
#include "Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

// classes
class InitialisationPartition : public niwa::Report {
public:
  InitialisationPartition(Model* model);
  virtual                     ~InitialisationPartition() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_CHILDREN_INITIALISATIONPARTITION_H_ */
