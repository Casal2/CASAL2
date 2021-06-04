/**
 * @file SimulatedObservation.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 8/05/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef REPORTS_SIMULATEDOBSERVATION_H_
#define REPORTS_SIMULATEDOBSERVATION_H_

// headers
#include "../../Reports/Report.h"

// namespace
namespace niwa {
class Observation;

namespace reports {

/**
 * class definition
 */
class SimulatedObservation : public Report {
public:
  SimulatedObservation();
  virtual ~SimulatedObservation() = default;
  void DoValidate(shared_ptr<Model> model) final{};
  void DoBuild(shared_ptr<Model> model) final;
  void DoExecute(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final{};

private:
  // members
  string       observation_label_ = "";
  Observation* observation_       = nullptr;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* REPORTS_SIMULATEDOBSERVATION_H_ */
