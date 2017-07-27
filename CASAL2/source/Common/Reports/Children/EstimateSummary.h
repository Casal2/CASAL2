/**
 * @file EstimateSummary.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 27/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print a summary of all of the estimate
 * objects that have been created in Casal2
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef ESTIMATESUMMARY_H_
#define ESTIMATESUMMARY_H_

// Headers
#include "Common/Reports/Report.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 * Class definition
 */
class EstimateSummary : public niwa::Report {
public:
  // Methods
  EstimateSummary(Model* model);
  virtual                     ~EstimateSummary() noexcept(true);
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };
};

} /* namespace reports */
} /* namespace niwa */
#endif /* ESTIMATESUMMARY_H_ */
