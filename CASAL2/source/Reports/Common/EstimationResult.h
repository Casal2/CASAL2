/**
 * @file EstimationResultResult.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 03/01/2019
 * @section LICENSE
 *
 * Copyright NIWA Science ©2019 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print the result of the minimiser
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef SOURCE_REPORTS_COMMON_ESTIMATIONRESULT_H_
#define SOURCE_REPORTS_COMMON_ESTIMATIONRESULT_H_

// Headers
#include "Reports/Report.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 * Class definition
 */
class EstimationResult : public niwa::Report {
public:
  // Methods
  EstimationResult(Model* model);
  virtual                     ~EstimationResult() noexcept(true);
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_COMMON_ESTIMATIONRESULT_H_ */
