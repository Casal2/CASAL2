/**
 * @file Factory.h
 * @author  C.Marsh
 * @date 16/11/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */
#ifndef AGEWEIGHTS_FACTORY_H_
#define AGEWEIGHTS_FACTORY_H_

// headers
#include "AgeWeights/AgeWeight.h"

// namespaces
namespace niwa {
class Model;

namespace ageweights {

// classes
class Factory {
public:
  static AgeWeight*           Create(Model* model, const string& object_type, const string& sub_type);

private:
  Factory() = delete;
  virtual ~Factory() = delete;
};



} /* namespace ageweights */
} /* namespace niwa */
#endif /* AGEWEIGHTS_FACTORY_H_ */
