/**
 * @file Project.h
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 29/03/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will take a target process and print all of
 * the values that have been assigned to it.
 */
#ifndef SOURCE_REPORTS_CHILDREN_PROCESS_H_
#define SOURCE_REPORTS_CHILDREN_PROCESS_H_

// headers
#include "Reports/Report.h"

// namespaces
namespace niwa {
class Process;
namespace reports {

// classes
class Project : public Report {
public:
  Project(Model* model);
  virtual                     ~Project() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };

private:
  string                      Project_label_ = "";
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_CHILDREN_PROCESS_H_ */
