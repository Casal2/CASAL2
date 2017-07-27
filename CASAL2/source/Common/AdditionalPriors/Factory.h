/**
 * @file Factory.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 24/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */
#ifndef ADDITIONAL_PRIORS_FACTORY_H_
#define ADDITIONAL_PRIORSFACTORY_H_

// headers
#include "Common/AdditionalPriors/AdditionalPrior.h"

// namespaces
namespace niwa {
class Model;

namespace additionalpriors {

// classes
class Factory {
public:
  // methods
  static AdditionalPrior*     Create(Model* model, const string& object_type, const string& sub_type);

private:
  // methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ADDITIONAL_PRIORS_FACTORY_H_ */
