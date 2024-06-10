/**
 * @file Manager.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/05/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_MCMCS_MANAGER_H_
#define SOURCE_MCMCS_MANAGER_H_

// headers
#include "../BaseClasses/Manager.h"
#include "../MCMCs/MCMC.h"

// namespaces
namespace niwa {
namespace mcmcs {

/**
 * Class definition
 */
class Manager : public niwa::base::Manager<niwa::mcmcs::Manager, niwa::MCMC> {
  friend class niwa::base::Manager<niwa::mcmcs::Manager, niwa::MCMC>;
  friend class niwa::Managers;

public:
  // methods
  Manager()          = default;
  virtual ~Manager() = default;
  void Validate() override final;
  void Validate(shared_ptr<Model> model);

  // accessors
  MCMC* active_mcmc() const { return mcmc_; }

private:
  // members
  MCMC* mcmc_          = nullptr;
  bool  has_validated_ = false;
};

} /* namespace mcmcs */
} /* namespace niwa */

#endif /* SOURCE_MCMCS_MANAGER_H_ */
