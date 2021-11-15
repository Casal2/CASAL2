/**
 * @file EstimationResultResult.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 03/01/2019
 * @section LICENSE
 *
 * Copyright NIWA Science �2019 - www.niwa.co.nz
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
#include "../../Reports/Report.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 * Class definition
 */
class EstimationResult : public niwa::Report {
public:
  // Methods
  EstimationResult();
  virtual ~EstimationResult() noexcept(true) = default;
  void DoValidate(shared_ptr<Model> model) final{};
  void DoBuild(shared_ptr<Model> model) final{};
  void DoExecute(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_COMMON_ESTIMATIONRESULT_H_ */
