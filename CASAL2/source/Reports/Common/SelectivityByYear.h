/**
 * @file SelectivityByYear.h
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 * @details print a selectivity during the year, useful for time-varying selectivities
 *
 */

#ifndef REPORTS_SELECTIVITY_BY_YEAR_H_
#define REPORTS_SELECTIVITY_BY_YEAR_H_

#include "../../Reports/Report.h"

namespace niwa {
class Selectivity;

namespace reports {

class SelectivityByYear : public niwa::Report {
public:
  SelectivityByYear();
  virtual ~SelectivityByYear() = default;
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
