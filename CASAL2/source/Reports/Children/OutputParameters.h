/**
 * @file OutputParameters.h
 * @author  C. Marsh
 * @version 1.0
 * @date 27/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report is specialised for the -o parameter, it prints out the estimated and profiled paramteter names and labels in -i format
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef OUTPUT_PARAMETERS_H_
#define OUTPUT_PARAMETERS_H_

// Headers
#include "Reports/Report.h"

// Namespaces
namespace niwa {
namespace reports {

/**
 * Class definition
 */
class OutputParameters : public niwa::Report {
public:
  // Methods
  OutputParameters(Model* model);
  virtual                     ~OutputParameters() noexcept(true);
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final { };
private:
  // members
  bool                        first_run_ = true;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* OUTPUT_PARAMETERS_H_ */
