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
namespace niwa {
namespace reports {

/**
 *
 */
class Observation : public niwa::Report {
public:
  // methods
  Observation(Model* model);
  virtual                     ~Observation() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        DoExecute() override final;

private:
  // members
  string              observation_label_;
  niwa::Observation*  observation_;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_OBSERVATION_H_ */
