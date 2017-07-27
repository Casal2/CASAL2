/**
 * @file Factory.h
 * @author  C. Marsh
 * @date 31/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifndef AGEINGERRORS_FACTORY_H_
#define AGEINGERRORS_FACTORY_H_

// headers
#include "Age/AgeingErrors/AgeingError.h"

// namespaces
namespace niwa {
class Model;

namespace ageingerrors {

// classes
class Factory {
public:
  static AgeingError*         Create(Model* model, const string& object_type, const string& sub_type);

private:
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace agesizes */
} /* namespace niwa */
#endif /* AGEINGERRORS_FACTORY_H_ */
