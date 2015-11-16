/**
 * @file Manager.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifndef ADDITIONAL_PRIORS_MANAGER_H_
#define ADDITIONAL_PRIORS_MANAGER_H_

// headers
#include "AdditionalPriors/AdditionalPrior.h"
#include "BaseClasses/Manager.h"
#include "Model/Managers.h"

// namespaces
namespace niwa {
namespace additionalpriors {

// classes
class Manager : public niwa::oldbase::Manager<niwa::additionalpriors::Manager, niwa::AdditionalPrior> {
  friend class niwa::oldbase::Manager<niwa::additionalpriors::Manager, niwa::AdditionalPrior>;
  friend class niwa::Managers;
public:
  // methods
  virtual                     ~Manager() = default;

protected:
  // methods
  Manager() {};
};

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ADDITIONAL_PRIORS_MANAGER_H_ */
