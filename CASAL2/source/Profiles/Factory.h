/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/03/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifndef PROFILES_FACTORY_H_
#define PROFILES_FACTORY_H_

// headers
#include <string>

#include "Profiles/Profile.h"

// namespaces
namespace niwa {
class Model;

namespace profiles {
using std::string;

/**
 * class definition
 */
class Factory {
public:
  // methods
  static Profile* Create(Model* model, const string& object_type, const string& sub_type);

private:
  // methods
  Factory() = delete;
  ~Factory() = delete;
};

} /* namespace profiles */
} /* namespace niwa */
#endif /* PROFILES_FACTORY_H_ */
