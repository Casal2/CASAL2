/*
 * Catchability.h
 *
 *  Created on: 4/09/2013
 *      Author: Admin
 */

#ifndef REPORTS_CATCHABILIY_H_
#define REPORTS_CATCHABILIY_H_

// headers
#include "Common/Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

/**
 *
 */
class Catchability : public niwa::Report {
public:
  Catchability(Model* model);
  virtual                     ~Catchability() = default;
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
