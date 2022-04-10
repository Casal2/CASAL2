/*
 * Catchability.h
 *
 *  Created on: 4/09/2013
 *      Author: Admin
 */

#ifndef REPORTS_CATCHABILITY_H_
#define REPORTS_CATCHABILITY_H_

// headers
#include "../../Catchabilities/Manager.h"
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
  void DoValidate(shared_ptr<Model> model) final;
  void DoBuild(shared_ptr<Model> model) final;
  void DoExecute(shared_ptr<Model> model) final;
  void DoPrepareTabular(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final;
  void DoFinaliseTabular(shared_ptr<Model> model) final;


private:
  string              unit_;
  string              catchability_label_ = "";
  niwa::Catchability* catchability_       = nullptr;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_CATCHABILITY_H_ */
