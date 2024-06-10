/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */
#ifndef AGELENGTHS_FACTORY_H_
#define AGELENGTHS_FACTORY_H_

// headers
#include "../AgeLengths/AgeLength.h"

// namespaces
namespace niwa {
class Model;

namespace agelengths {

// classes
class Factory {
public:
  static AgeLength* Create(shared_ptr<Model> model, const string& object_type, const string& sub_type);

private:
  Factory()          = delete;
  virtual ~Factory() = delete;
};

}  // namespace agelengths
} /* namespace niwa */
#endif /* AGELENGTHS_FACTORY_H_ */
