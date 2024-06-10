/**
 * @file Manager.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/10/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */
#ifndef ADDITIONAL_PRIORS_MANAGER_H_
#define ADDITIONAL_PRIORS_MANAGER_H_

// headers
#include "../AdditionalPriors/AdditionalPrior.h"
#include "../BaseClasses/Manager.h"
#include "../Model/Managers.h"

// namespaces
namespace niwa {
namespace additionalpriors {

// classes
class Manager : public niwa::base::Manager<niwa::additionalpriors::Manager, niwa::AdditionalPrior> {
  friend class niwa::base::Manager<niwa::additionalpriors::Manager, niwa::AdditionalPrior>;
  friend class niwa::Managers;

public:
  // methods
  virtual ~Manager() = default;
  bool             HasAdditionalPrior(string_view parameter);
  AdditionalPrior* GetAdditionalPrior(string_view parameter);
  AdditionalPrior* GetAdditionalPriorExcludingRatioType(string_view parameter);
  bool             HasAdditionalPriorExcludingRatioType(string_view parameter);


protected:
  // methods
  Manager() = default;
};

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ADDITIONAL_PRIORS_MANAGER_H_ */
