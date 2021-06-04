/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef OBSERVATIONS_FACTORY_H_
#define OBSERVATIONS_FACTORY_H_

// Headers
#include "Observations/Observation.h"

// Namespaces
namespace niwa {
class Model;

namespace observations {

/**
 * Class Definition
 */
class Factory {
public:
  // methods
  static Observation* Create(shared_ptr<Model> model, const string& object_type, const string& sub_type);

private:
  // Methods
  Factory()          = delete;
  virtual ~Factory() = delete;
};

} /* namespace observations */
} /* namespace niwa */
#endif /* OBSERVATIONS_FACTORY_H_ */
