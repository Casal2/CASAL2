/**
 * @file Observation.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 21/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This object will print the details of the observation including all comparisons
 */
#ifndef REPORTS_OBSERVATION_H_
#define REPORTS_OBSERVATION_H_

// headers
#include "Reports/Report.h"
#include "Observations/Observation.h"

// namespaces
namespace isam {
namespace reports {

/**
 *
 */
class Observation : public isam::Report {
public:
  // methods
  Observation();
  virtual                     ~Observation() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        Execute() override final;

private:
  // members
  string              observation_label_;
  ObservationPtr      observation_;
};

} /* namespace reports */
} /* namespace isam */
#endif /* REPORTS_OBSERVATION_H_ */
