/*
 * Catchabilities.h
 *
 *  Created on: 4/09/2013
 *      Author: Admin
 */

#ifndef REPORTS_CATCHABILIY_H_
#define REPORTS_CATCHABILIY_H_

// headers
#include "Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

/**
 *
 */
class Catchabilities : public niwa::Report {
public:
  Catchabilities(Model* model);
  virtual                     ~Catchabilities() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
  void                        DoExecute() override final;
  void                        DoExecuteTabular() override final;
  void                        DoFinaliseTabular() override final;


private:
  bool						  first_run_ = true;
  string            unit_;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_CATCHABILIY_H_ */
