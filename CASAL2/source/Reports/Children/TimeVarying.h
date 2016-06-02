/**
 * @file TimeVarying.h
 * @author  C. Marsh
 * @date 01/06/2016
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 */

#ifndef REPORTS_TIMEVARYING_H_
#define REPORTS_TIMEVARYING_H_

// headers
#include "Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

/**
 *
 */
class TimeVarying : public niwa::Report {
public:
  TimeVarying(Model* model);
  virtual                     ~TimeVarying() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final;
  void                        DoFinaliseTabular() override final;


private:
  bool						  first_run_ = true;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_TIMEVARYING_H_ */
