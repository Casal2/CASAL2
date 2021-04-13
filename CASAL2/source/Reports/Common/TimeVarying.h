/**
 * @file TimeVarying.h
 * @author  C. Marsh
 * @date 01/06/2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */

#ifndef REPORTS_TIMEVARYING_H_
#define REPORTS_TIMEVARYING_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

/**
 *
 */
class TimeVarying : public niwa::Report {
public:
  TimeVarying();
  virtual                     ~TimeVarying() = default;
  void                        DoValidate(shared_ptr<Model> model) final { };
  void                        DoBuild(shared_ptr<Model> model) final { };
  void                        DoExecute(shared_ptr<Model> model) final;
  void                        DoExecuteTabular(shared_ptr<Model> model) final;
  void                        DoFinaliseTabular(shared_ptr<Model> model) final;


private:
  bool                        first_run_ = true;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_TIMEVARYING_H_ */
