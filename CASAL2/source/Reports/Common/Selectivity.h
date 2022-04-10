/*
 * Selectivity.h
 *
 *  Created on: 25/06/2014
 *      Author: Admin
 */

#ifndef REPORTS_SELECTIVITY_H_
#define REPORTS_SELECTIVITY_H_

#include "../../Reports/Report.h"

namespace niwa {
class Selectivity;

namespace reports {

class Selectivity : public niwa::Report {
public:
  Selectivity();
  virtual ~Selectivity() = default;
  void DoValidate(shared_ptr<Model> model) final;
  void DoBuild(shared_ptr<Model> model) final;
  void DoPrepareTabular(shared_ptr<Model> model) final;
  void DoPrepare(shared_ptr<Model> model) final{};
  void DoExecute(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final;
  void DoFinalise(shared_ptr<Model> model) final{};
  void DoFinaliseTabular(shared_ptr<Model> model) final;

private:
  string             selectivity_label_;
  niwa::Selectivity* selectivity_ = nullptr;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SELECTIVITY_H_ */
