/**
 * @file MCMCSample.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 5/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report prints out the sample information from our MCMC chain
 */
#ifndef SOURCE_REPORTS_CHILDREN_MCMCSAMPLE_H_
#define SOURCE_REPORTS_CHILDREN_MCMCSAMPLE_H_

// headers
#include "Reports/Report.h"

// namespaces
namespace niwa {
class MCMC;

namespace reports {

// class
class MCMCSample : public niwa::Report {
public:
  MCMCSample(Model* model);
  virtual                     ~MCMCSample() = default;
  void                        DoValidate() override final { };
  void                        DoBuild() override final;
  void                        DoPrepare() override final;
  void                        DoExecute() override final;
  void                        DoFinalise() override final;
  void                        DoExecuteTabular() override final { };

private:
  MCMC*                       mcmc_ = nullptr;
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_CHILDREN_MCMCSAMPLE_H_ */
