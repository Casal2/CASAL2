/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 13/12/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */
#ifndef INITIALISATIONPHASE_FACTORY_H_
#define INITIALISATIONPHASE_FACTORY_H_

// Headers
#include <string>

#include "../InitialisationPhases/InitialisationPhase.h"

// Namespaces
namespace niwa {
class Model;

namespace initialisationphases {
using std::string;

/**
 * Class definition
 */
class Factory {
public:
  // methods
  static InitialisationPhase* Create(shared_ptr<Model> model, const string& object_type, const string& sub_type);

private:
  // methods
  Factory()          = delete;
  virtual ~Factory() = delete;
};

}  // namespace initialisationphases
} /* namespace niwa */
#endif /* INITIALISATIONPHASE_FACTORY_H_ */
