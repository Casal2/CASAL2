/**
 * @file Manager.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Oct 26, 2016
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef LIKELIHOODS_MANAGER_H_
#define LIKELIHOODS_MANAGER_H_

// headers
#include "../BaseClasses/Manager.h"
#include "../Likelihoods/Likelihood.h"

// namespaces
namespace niwa {
class Model;
namespace likelihoods {

/**
 * class definition
 */
class Manager : public niwa::base::Manager<niwa::likelihoods::Manager, niwa::Likelihood> {
  friend class niwa::base::Manager<niwa::likelihoods::Manager, niwa::Likelihood>;
  friend class niwa::Managers;

public:
  // methods
  virtual ~Manager() noexcept(true){};
  Likelihood* GetOrCreateLikelihood(shared_ptr<Model> model, const string& observation_label, const string& label);
  Likelihood* GetLikelihood(const string& label);

protected:
  // methods
  Manager();
};

} /* namespace likelihoods */
} /* namespace niwa */
#endif /* LIKELIHOODS_MANAGER_H_ */
