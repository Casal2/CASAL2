/**
 * @file Factory.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 24/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifndef ADDITIONAL_PRIORS_FACTORY_H_
#define ADDITIONAL_PRIORSFACTORY_H_

// headers
#include "AdditionalPriors/AdditionalPrior.h"

// namespaces
namespace isam {
namespace additionalpriors {

/**
 * Class definition
 */
class Factory {
public:
  static AdditionalPriorPtr Create(string object_type, string sub_type);

private:
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace additionalpriors */
} /* namespace isam */

#endif /* FACTORY_H_ */
