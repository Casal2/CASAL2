/**
 * @file Process.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 19/11/2015
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
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
class Process;
namespace reports {

// classes
class Process : public Report {
public:
  Process();
  virtual                     ~Process() = default;
  void                        DoValidate(shared_ptr<Model> model) final { };
  void                        DoBuild(shared_ptr<Model> model) final;
  void                        DoExecute(shared_ptr<Model> model) final;
  void                        DoExecuteTabular(shared_ptr<Model> model) final;
  void                        DoFinaliseTabular(shared_ptr<Model> model) final;

private:
  string                      process_label_ = "";
  niwa::Process*              process_ = nullptr;
  bool                        first_run_ = true;

};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_CHILDREN_PROCESS_H_ */
