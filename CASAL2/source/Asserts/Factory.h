/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/08/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */
#ifndef ASSERTS_FACTORY_H_
#define ASSERTS_FACTORY_H_

// headers
#include <string>

#include "../Asserts/Assert.h"

// namespaces
namespace niwa {
class Model;

namespace asserts {
using std::string;

// classes
class Factory {
public:
  // methods
  static Assert* Create(shared_ptr<Model> model, const string& object_type, const string& sub_type);

private:
  // methods
  Factory()          = delete;
  virtual ~Factory() = delete;
};

} /* namespace asserts */
} /* namespace niwa */
#endif /* ASSERTS_FACTORY_H_ */
