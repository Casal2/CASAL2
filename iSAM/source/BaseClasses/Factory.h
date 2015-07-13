/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/10/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a base template factory class that allows
 * the easy creation of new objects based on their parent type
 * and a string-type parameter
 *
 */
#ifndef BASE_FACTORY_H_
#define BASE_FACTORY_H_

// Headers
#include <memory>

#include "Model/Model.h"

// namespaces
namespace niwa {
namespace base {

using std::shared_ptr;

// classes
template<class ObjectType, class ManagerType>
class Factory {
public:
  // methods
  static shared_ptr<ObjectType> Create() {
    shared_ptr<ObjectType> return_value = shared_ptr<ObjectType>(new ObjectType());

    ManagerType::Instance().AddObject(return_value);
    return return_value;
  }

private:
  // Methods
  Factory() = delete;;
  virtual ~Factory() = delete;;
};

} /* namespace base */
} /* namespace niwa */
#endif /* BASE_FACTORY_H_ */
