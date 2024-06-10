/**
 * @file Factory.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date Dec 7, 2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SOURCE_ESTIMABLETRANSFORMATIONS_FACTORY_H_
#define SOURCE_ESTIMABLETRANSFORMATIONS_FACTORY_H_

// headers
#include <string>

#include "AddressableTransformation.h"

// namespaces
namespace niwa {
namespace addressabletransformations {

// classes
class Factory {
public:
  static AddressableTransformation* Create(shared_ptr<Model> model, const string& object_type, const string& sub_type);

private:
  Factory()          = delete;
  virtual ~Factory() = delete;
};

} /* namespace addressabletransformations */
} /* namespace niwa */

#endif /* SOURCE_ESTIMABLETRANSFORMATIONS_FACTORY_H_ */
