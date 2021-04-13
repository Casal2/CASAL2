/**
 * @file MPD.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 30/10/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This report will dump the covariance matrix and addressables in a way that
 * can be read back in to Casal2 for use in MCMC or a new estimation
 */
#ifndef SOURCE_REPORTS_CHILDREN_MPD_H_
#define SOURCE_REPORTS_CHILDREN_MPD_H_

// headers
#include "../../Reports/Report.h"

// namespaces
namespace niwa {
namespace reports {

// classes
class MPD : public Report {
public:
  MPD();
  virtual                     ~MPD() = default;
  void                        DoValidate(shared_ptr<Model> model) final { };
  void                        DoBuild(shared_ptr<Model> model) final { };
  void                        DoExecute(shared_ptr<Model> model) final;
  void                        DoExecuteTabular(shared_ptr<Model> model) final { };
};

} /* namespace reports */
} /* namespace niwa */

#endif /* SOURCE_REPORTS_CHILDREN_MPD_H_ */
