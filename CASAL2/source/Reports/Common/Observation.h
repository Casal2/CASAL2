/**
 * @file Observation.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 21/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This object will print the details of the observation including all comparisons
 */
#ifndef REPORTS_OBSERVATION_H_
#define REPORTS_OBSERVATION_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
class Observation;

namespace reports {

/**
 *
 */
class Observation : public niwa::Report {
public:
  // methods
  Observation();
  virtual ~Observation() = default;
  void DoValidate(shared_ptr<Model> model) final;
  void DoBuild(shared_ptr<Model> model) final;
  void DoExecute(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final;
  void DoFinaliseTabular(shared_ptr<Model> model) final;

private:
  // members
  string             observation_label_ = "";
  niwa::Observation* observation_       = nullptr;
  bool               normalised_resids_ = true;
  bool               pearson_resids_    = true;
  bool               first_run_         = true;
};

} /* namespace reports */
} /* namespace niwa */
#endif /* REPORTS_OBSERVATION_H_ */
