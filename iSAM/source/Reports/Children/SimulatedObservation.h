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
#include "Reports/Report.h"

// namespace
namespace niwa {
class Observation;

namespace reports {

/**
 * class definition
 */
class SimulatedObservation: public Report {
public:
  SimulatedObservation(Model* model);
  virtual                     ~SimulatedObservation() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        DoExecute() override final;

private:
  // members
  string                      observation_label_;
  Observation*                observation_;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* REPORTS_SIMULATEDOBSERVATION_H_ */
