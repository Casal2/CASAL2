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
#include "Observations/Observation.h"
#include "Reports/Report.h"


// namespace
namespace isam {
namespace reports {

/**
 * class definition
 */
class SimulatedObservation: public Report {
public:
  SimulatedObservation();
  virtual                     ~SimulatedObservation() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        Execute() override final;

private:
  // members
  string                      observation_label_;
  ObservationPtr              observation_;
};

} /* namespace reports */
} /* namespace isam */

#endif /* REPORTS_SIMULATEDOBSERVATION_H_ */
