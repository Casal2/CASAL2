/**
 * @file fEquationTest.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/11/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This class allows you to run a test equation against the model during the finalise phase.
 */
#ifndef SOURCE_REPORTS_CHILDREN_EQUATIONTEST_H_
#define SOURCE_REPORTS_CHILDREN_EQUATIONTEST_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

// class
class EquationTest : public niwa::Report {
public:
  EquationTest();
  virtual ~EquationTest() = default;
  void DoValidate(shared_ptr<Model> model) final{};
  void DoBuild(shared_ptr<Model> model) final{};
  void DoExecute(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final;

private:
  // members
  vector<string> equation_input_;
  string         equation_ = "";
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_CHILDREN_EQUATIONTEST_H_ */
