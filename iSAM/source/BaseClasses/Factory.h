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
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef BASE_FACTORY_H_
#define BASE_FACTORY_H_

// Headers
#include <boost/shared_ptr.hpp>

#include "Model/Model.h"

// namespaces
namespace isam {
namespace base {

using boost::shared_ptr;

/**
 * Class definition
 */
template<class ObjectType, class ManagerType>
class Factory {
public:
  // Methods
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
} /* namespace isam */
#endif /* BASE_FACTORY_H_ */
