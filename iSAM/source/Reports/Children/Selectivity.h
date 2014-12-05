/*
 * Selectivity.h
 *
 *  Created on: 25/06/2014
 *      Author: Admin
 */

#ifndef REPORTS_SELECTIVITY_H_
#define REPORTS_SELECTIVITY_H_

#include "Reports/Report.h"
#include "Selectivities/Manager.h"

namespace niwa {
namespace reports {

class Selectivity : public niwa::Report {
public:
  Selectivity();
  virtual ~Selectivity() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoPrepare() override final { };
  void                        DoExecute() override final;
  void                        DoFinalise() override final { };

private:
  string                      selectivity_label_;
  SelectivityPtr              selectivity_;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SELECTIVITY_H_ */
