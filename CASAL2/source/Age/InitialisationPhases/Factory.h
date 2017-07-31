/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef AGE_INITIALISATIONPHASES_FACTORY_H_
#define AGE_INITIALISATIONPHASES_FACTORY_H_

// Headers
#include "Common/InitialisationPhases/InitialisationPhase.h"
#include "Common/InitialisationPhases/Manager.h"

// Namespaces
namespace niwa {
class Model;

namespace age {
namespace initialisationphases {

/**
 * Class Definition
 */
class Factory {
public:
  // methods
  static InitialisationPhase* Create(Model* model, const string& object_type, const string& sub_type);

private:
  // methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace initialisationphases */
} /* namespace age */
} /* namespace niwa */

#endif /* AGE_INITIALISATIONPHASES_FACTORY_H_ */
