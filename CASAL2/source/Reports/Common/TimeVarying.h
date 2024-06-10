/**
 * @file TimeVarying.h
 * @author  C. Marsh
 * @date 01/06/2016
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

#ifndef REPORTS_TIMEVARYING_H_
#define REPORTS_TIMEVARYING_H_

// headers
#include "../../Reports/Report.h"
#include "../../TimeVarying/Manager.h"

// namespaces
namespace niwa {
namespace reports {

/**
 *
 */
class TimeVarying : public niwa::Report {
public:
  TimeVarying();
  virtual ~TimeVarying() = default;
  void DoValidate(shared_ptr<Model> model) final;
  void DoBuild(shared_ptr<Model> model) final;
  void DoExecute(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final;
  void DoFinaliseTabular(shared_ptr<Model> model) final;
  void DoPrepareTabular(shared_ptr<Model> model) final;

private:
  string             time_varying_label_ = "";
  bool               first_run_          = true;
  niwa::TimeVarying* timevarying_        = nullptr;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_TIMEVARYING_H_ */
