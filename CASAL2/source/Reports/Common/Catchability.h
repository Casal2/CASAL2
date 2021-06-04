/*
 * Catchability.h
 *
 *  Created on: 4/09/2013
 *      Author: Admin
 */

#ifndef REPORTS_CATCHABILIY_H_
#define REPORTS_CATCHABILIY_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

/**
 *
 */
class Catchability : public niwa::Report {
public:
  Catchability();
  virtual ~Catchability() = default;
  void DoValidate(shared_ptr<Model> model) final{};
  void DoBuild(shared_ptr<Model> model) final{};
  void DoExecute(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final;
  void DoFinaliseTabular(shared_ptr<Model> model) final;

private:
  bool   first_run_ = true;
  string unit_;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_CATCHABILIY_H_ */
