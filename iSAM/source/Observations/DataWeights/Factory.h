/**
 * @file Factory.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 22/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */
#ifndef SOURCE_OBSERVATIONS_DATAWEIGHTS_FACTORY_H_
#define SOURCE_OBSERVATIONS_DATAWEIGHTS_FACTORY_H_

// headers
#include <string>

#include "Observations/DataWeights/DataWeight.h"

// namespaces
namespace niwa {
namespace observations {
namespace dataweights {

using std::cout;

/**
 * Class definition
 */
class Factory {
public:
  // methods
  static DataWeightPtr Create(const std::string& type);

private:
  // methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace dataweights */
} /* namespace observations */
} /* namespace niwa */

#endif /* SOURCE_OBSERVATIONS_DATAWEIGHTS_FACTORY_H_ */
