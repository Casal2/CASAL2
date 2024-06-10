/**
 * @file Project.h
 * @author C.Marsh
 * @date 5/6/17
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This report will take a target a project and print all of
 * the values that have been assigned to it.
 */
#ifndef SOURCE_REPORTS_CHILDREN_PROJECT_H_
#define SOURCE_REPORTS_CHILDREN_PROJECT_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
class Project;
namespace reports {

// classes
class Project : public Report {
public:
  Project();
  virtual ~Project() = default;
  void DoValidate(shared_ptr<Model> model) override final;
  void DoBuild(shared_ptr<Model> model) override final;
  void DoExecute(shared_ptr<Model> model) override final;
  void DoPrepareTabular(shared_ptr<Model> model) override final;
  void DoExecuteTabular(shared_ptr<Model> model) override final;
  void DoFinaliseTabular(shared_ptr<Model> model) override final;

private:
  string         project_label_ = "";
  niwa::Project* project_       = nullptr;
  bool           first_time_    = true;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_CHILDREN_PROJECT_H_ */
