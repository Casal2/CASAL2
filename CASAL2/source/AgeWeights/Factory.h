/**
 * @file Factory.h
 * @author  C.Marsh
 * @date 16/11/2017
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */
#ifndef AGEWEIGHTS_FACTORY_H_
#define AGEWEIGHTS_FACTORY_H_

// headers
#include "../AgeWeights/AgeWeight.h"

// namespaces
namespace niwa {
class Model;

namespace ageweights {

// classes
class Factory {
public:
  static AgeWeight* Create(shared_ptr<Model> model, const string& object_type, const string& sub_type);

private:
  Factory()          = delete;
  virtual ~Factory() = delete;
};

} /* namespace ageweights */
} /* namespace niwa */
#endif /* AGEWEIGHTS_FACTORY_H_ */
