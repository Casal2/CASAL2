/**
 * @file Object.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 7/10/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is an implementation of the Factory pattern that will create
 * a shared_ptr to an "Object" derived class. The majority of functional classes
 * in this application inherit from Object so this factory will be responsible
 * for creating the majority of instances during any run.
 */
#ifndef FACTORY_OBJECT_H_
#define FACTORY_OBJECT_H_

// headers
#include <string>

#include "BaseClasses/Object.h"

// namespaces
namespace isam {
namespace factory {

using std::string;

/**
 * Class definition
 */
class Object {
public:
  // methods
  static base::ObjectPtr      Create(const string& object_type, const string& sub_type);

private:
  // methods
  Object() = delete;
  virtual ~Object() = delete;
};

} /* namespace factory */
} /* namespace isam */
#endif /* OBJECT_H_ */
