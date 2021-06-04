/**
 * @file MCMCCovariance.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 15/05/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will print the MCMC chain. It won't print it all at once but it will instead
 * stream it out over time so it can be restarted if necessary
 */
#ifndef MCMCCOVARIANCE_H_
#define MCMCCOVARIANCE_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

class MCMCCovariance : public niwa::Report {
public:
  MCMCCovariance();
  virtual ~MCMCCovariance() = default;
  void DoValidate(shared_ptr<Model> model) final{};
  void DoBuild(shared_ptr<Model> model) final{};
  void DoExecute(shared_ptr<Model> model) final;
  void DoExecuteTabular(shared_ptr<Model> model) final{};
};

} /* namespace reports */
} /* namespace niwa */
#endif /* MCMCCOVARIANCE_H_ */
