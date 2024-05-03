/*
 * Initialisation.h
 *
 *  Created on: 01/02/2023
 *      Author: Alistair Dunn
 */

#ifndef REPORTS_INITIALISATION_H_
#define REPORTS_INITIALISATION_H_

#include "../../InitialisationPhases/Manager.h"
#include "../../Reports/Report.h"

namespace niwa {
namespace reports {

class Initialisation : public niwa::Report {
public:
  Initialisation();
  virtual ~Initialisation() = default;
  void DoValidate(shared_ptr<Model> model) final{};
  void DoBuild(shared_ptr<Model> model) final{};
  void DoExecute(shared_ptr<Model> model) final;
  void DoFinalise(shared_ptr<Model> model) final{};

  void DoPrepareTabular(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final;
  void DoFinaliseTabular(shared_ptr<Model> model) final;

private:
  niwa::InitialisationPhase* initialisation_ = nullptr;
  vector<Double>             lambda_;
  vector<unsigned>           years_;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* REPORTS_INITIALISATION_H_ */
