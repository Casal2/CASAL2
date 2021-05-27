/**
 * @file IndependenceMetropolis.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/05/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_MCMCS_COMMON_INDEPENDENCEMETROPOLIS_H_
#define SOURCE_MCMCS_COMMON_INDEPENDENCEMETROPOLIS_H_
#ifndef USE_AUTODIFF

// headers
#include "../../Estimates/Estimate.h"
#include "../../MCMCs/MCMC.h"

// namespaces
namespace niwa {
namespace mcmcs {
/**
 * Class definition
 */
class IndependenceMetropolis : public MCMC {
public:
  // methods
  IndependenceMetropolis(shared_ptr<Model> model);
  virtual ~IndependenceMetropolis() = default;
  void DoExecute(shared_ptr<ThreadPool> thread_pool) override final;

protected:
  // methods
  void DoValidate() final{};
  void DoBuild() final{};

  // members
};

} /* namespace mcmcs */
} /* namespace niwa */

#endif  // USE_AUTODIFF
#endif  /* SOURCE_MCMCS_COMMON_INDEPENDENCEMETROPOLIS_H_ */
