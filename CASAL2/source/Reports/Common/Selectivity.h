/*
 * Selectivity.h
 *
 *  Created on: 25/06/2014
 *      Author: Admin
 */

#ifndef REPORTS_SELECTIVITY_H_
#define REPORTS_SELECTIVITY_H_

#include "Reports/Report.h"

namespace niwa {
class Selectivity;

namespace reports {

class Selectivity : public niwa::Report {
public:
  Selectivity(Model* model);
  virtual ~Selectivity() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoPrepare() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final;
  void                        DoFinalise() override final { };
  void                        DoFinaliseTabular() override final;

private:
  string                      selectivity_label_;
  niwa::Selectivity*          selectivity_;
  bool                        first_run_ = true;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SELECTIVITY_H_ */
