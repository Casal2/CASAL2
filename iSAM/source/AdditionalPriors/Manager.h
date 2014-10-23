/**
 * @file Manager.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 21/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef ADDITIONAL_PRIORS_MANAGER_H_
#define ADDITIONAL_PRIORS_MANAGER_H_

// headers
#include "AdditionalPriors/AdditionalPrior.h"
#include "BaseClasses/Manager.h"

// namespaces
namespace isam {
namespace additionalpriors {

/**
 * Class definition
 */
class Manager : public isam::oldbase::Manager<isam::additionalpriors::Manager, isam::AdditionalPrior> {
  friend class isam::oldbase::Manager<isam::additionalpriors::Manager, isam::AdditionalPrior>;
public:
  // methods
  virtual                     ~Manager() = default;

protected:
  // methods
  Manager() {};

};

} /* namespace additionalpriors */
} /* namespace isam */

#endif /* ADDITIONAL_PRIORS_MANAGER_H_ */
