/**
 * @file MCMCObjective.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_REPORTS_CHILDREN_MCMCOBJECTIVE_H_
#define SOURCE_REPORTS_CHILDREN_MCMCOBJECTIVE_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
class MCMC;

namespace reports {

// class
class MCMCObjective : public Report {
public:
  MCMCObjective();
  virtual ~MCMCObjective() = default;
  void DoValidate(shared_ptr<Model> model) final{};
  void DoBuild(shared_ptr<Model> model) final;
  void DoPrepare(shared_ptr<Model> model) final;
  void DoExecute(shared_ptr<Model> model) final;
  void DoFinalise(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final{};

private:
  MCMC* mcmc_       = nullptr;
  bool  first_time_ = true;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_CHILDREN_MCMCOBJECTIVE_H_ */
