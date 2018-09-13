/**
 * @file fEquationTest.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class allows you to run a test equation against the model during the finalise phase.
 */
#ifndef SOURCE_REPORTS_CHILDREN_EQUATIONTEST_H_
#define SOURCE_REPORTS_CHILDREN_EQUATIONTEST_H_

// headers
#include "Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

// class
class EquationTest : public niwa::Report {
public:
  EquationTest() = delete;
  explicit EquationTest(Model* model);
  virtual                     ~EquationTest() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };

private:
  // members
  vector<string>              equation_input_;
  string                      equation_ = "";
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_CHILDREN_EQUATIONTEST_H_ */
