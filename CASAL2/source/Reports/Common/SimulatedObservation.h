/**
 * @file SimulatedObservation.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 8/05/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
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
  void DoValidate(shared_ptr<Model> model) final;
  void DoBuild(shared_ptr<Model> model) final;
  void DoExecute(shared_ptr<Model> model) final;
  void DoPrepareTabular(shared_ptr<Model> model) final;

private:
  // members
  string       observation_label_ = "";
  bool         print_all_observations = false;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* REPORTS_SIMULATEDOBSERVATION_H_ */
