/**
 * @file Factory.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 20/05/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */
#ifndef SOURCE_MCMCS_FACTORY_H_
#define SOURCE_MCMCS_FACTORY_H_

// headers
#include <string>

#include "MCMCs/MCMC.h"

// namespaces
namespace niwa {
class Model;

namespace mcmcs {

/**
 * Class definition
 */
class Factory {
public:
  // methods
  static MCMC* Create(Model* model, const string& object_type, const string& sub_type);

private:
  // methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace mcmcs */
} /* namespace niwa */

#endif /* SOURCE_MCMCS_FACTORY_H_ */
