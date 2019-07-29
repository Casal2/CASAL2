/**
 * @file ObjectiveFunction.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 21/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print the results from the
 * objective function
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef REPORTS_OBJECTIVEFUNCTION_H_
#define REPORTS_OBJECTIVEFUNCTION_H_

// Headers
#include "Reports/Report.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 * Class definition
 */
class ObjectiveFunction : public niwa::Report {
public:
  // Methods
  ObjectiveFunction(Model* model);
  virtual                     ~ObjectiveFunction() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_OBJECTIVEFUNCTION_H_ */
