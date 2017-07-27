/**
 * @file Project.h
 * @author C.Marsh
 * @date 5/6/17
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will take a target a project and print all of
 * the values that have been assigned to it.
 */
#ifndef SOURCE_REPORTS_CHILDREN_PROJECT_H_
#define SOURCE_REPORTS_CHILDREN_PROJECT_H_

// headers
#include "Common/Reports/Report.h"

// namespaces
namespace niwa {
class Project;
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
  string                      project_label_ = "";
  niwa::Project*              project_ = nullptr;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_CHILDREN_PROJECT_H_ */
