/*
 * DefaultReports.h
 *
 *  Created on: 4/09/2013
 *      Author: Admin
 */

#ifndef REPORTS_DEFAULTREPORTS_H_
#define REPORTS_DEFAULTREPORTS_H_

// headers
#include "../../Catchabilities/Manager.h"
#include "../../DerivedQuantities/Manager.h"
#include "../../Observations/Manager.h"
#include "../../Processes/Manager.h"
#include "../../Projects/Manager.h"
#include "../../Reports/Common/Catchability.h"
#include "../../Reports/Common/DerivedQuantity.h"
#include "../../Reports/Common/Observation.h"
#include "../../Reports/Common/AddressableTransformation.h"
#include "../../Reports/Common/Process.h"
#include "../../Reports/Common/Project.h"
#include "../../Reports/Common/Selectivity.h"
#include "../../Reports/Manager.h"
#include "../../Reports/Report.h"
#include "../../Selectivities/Manager.h"

// namespaces
namespace niwa {
namespace reports {

/**
 *
 */
class DefaultReports : public niwa::Report {
public:
  DefaultReports();
  virtual ~DefaultReports() = default;
  void DoValidate(shared_ptr<Model> model) final{};
  void DoBuild(shared_ptr<Model> model) final;
  void DoExecute(shared_ptr<Model> model) final{};

private:
  bool   first_run_ = true;
  string unit_;
  bool   report_catchabilities_     = false;
  bool   report_derived_quantities_ = false;
  bool   report_observations_       = false;
  bool   report_processes_          = false;
  bool   report_projects_           = false;
  bool   report_selectivities_      = false;
  bool   addressable_transformations_ = false;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* DEFAULTREPORTS_H_ */
