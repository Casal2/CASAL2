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
#include "Reports/Report.h"

// namespaces
namespace niwa {
class Process;
namespace reports {

// classes
class Process : public Report {
public:
  Process(Model* model);
  virtual                     ~Process() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        DoExecute() override final;

private:
  string                      process_label_ = "";
  niwa::Process*              process_ = nullptr;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_CHILDREN_PROCESS_H_ */
