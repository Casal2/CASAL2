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
#ifndef SOURCE_OBSERVATIONS_PROCESSERRORS_FACTORY_H_
#define SOURCE_OBSERVATIONS_PROCESSERRORS_FACTORY_H_

// headers
#include <string>

#include "Observations/ProcessErrors/ProcessError.h"

// namespaces
namespace niwa {
namespace observations {
namespace processerrors {

using std::cout;

/**
 * Class definition
 */
class Factory {
public:
  // methods
  static ProcessErrorPtr Create(const std::string& type);

private:
  // methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace processerrors */
} /* namespace observations */
} /* namespace niwa */

#endif /* SOURCE_OBSERVATIONS_PROCESSERRORS_FACTORY_H_ */
