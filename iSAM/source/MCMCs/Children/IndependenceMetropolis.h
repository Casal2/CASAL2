/**
 * @file IndependenceMetropolis.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/05/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_MCMCS_CHILDREN_INDEPENDENCEMETROPOLIS_H_
#define SOURCE_MCMCS_CHILDREN_INDEPENDENCEMETROPOLIS_H_

// headers
#include "MCMCs/MCMC.h"

// namespaces
namespace niwa {
namespace mcmcs {

/**
 * Class definition
 */
class IndependenceMetropolis : public MCMC {
public:
  // methods
  IndependenceMetropolis(Model* model);
  virtual                     ~IndependenceMetropolis() = default;
  void                        Execute() override final;

protected:
  // methods
  void                        DoValidate() override final { };
  void                        DoBuild() override final { };
};

} /* namespace mcmcs */
} /* namespace niwa */

#endif /* SOURCE_MCMCS_CHILDREN_INDEPENDENCEMETROPOLIS_H_ */
