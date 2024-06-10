/**
 * @file Factory.h
 * @author  C. Marsh
 * @date 31/08/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */
#ifndef AGEINGERRORS_FACTORY_H_
#define AGEINGERRORS_FACTORY_H_

// headers
#include "../AgeingErrors/AgeingError.h"

// namespaces
namespace niwa {
class Model;

namespace ageingerrors {

// classes
class Factory {
public:
  static AgeingError* Create(shared_ptr<Model> model, const string& object_type, const string& sub_type);

private:
  Factory()          = delete;
  virtual ~Factory() = delete;
};

}  // namespace ageingerrors
} /* namespace niwa */
#endif /* AGEINGERRORS_FACTORY_H_ */
