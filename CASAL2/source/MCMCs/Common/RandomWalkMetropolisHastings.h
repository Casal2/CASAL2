/**
 * @file RandomWalkMetropolisHastings.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-04-24
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef SOURCE_MCMCS_COMMON_RANDOMWALKMETROPOLISHASTINGS_H_
#define SOURCE_MCMCS_COMMON_RANDOMWALKMETROPOLISHASTINGS_H_
#ifndef USE_AUTODIFF

// headers
#include "../../Estimates/Estimate.h"
#include "../../MCMCs/MCMC.h"

// namespaces
namespace niwa::mcmcs {

class RandomWalkMetropolisHastings : public MCMC {
public:
  // methods
  RandomWalkMetropolisHastings(shared_ptr<Model> model);
  virtual ~RandomWalkMetropolisHastings() = default;
  void DoExecute(shared_ptr<ThreadPool> thread_pool) final;

protected:
  // methods
  void DoValidate() final{};
  void DoBuild() final{};
};  // class RandomWalkMetropolisHastings
}  // namespace niwa::mcmcs

#endif  // USE_AUTODIFF
#endif  // SOURCE_MCMCS_COMMON_RANDOMWALKMETROPOLISHASTINGS_H_