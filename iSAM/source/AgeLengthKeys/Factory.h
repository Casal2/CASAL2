/**
 * @file Factory.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 12/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Factory object for Age Length Keys
 */
#ifndef SOURCE_AGELENGTHKEYS_FACTORY_H_
#define SOURCE_AGELENGTHKEYS_FACTORY_H_

// headers
#include "AgeLengthKeys/AgeLengthKey.h"

// namespaces
namespace niwa {
namespace agelengthkeys {

// classes
class Factory {
public:
  // methods
  static AgeLengthKeyPtr      Create(const string& object_type, const string& sub_type);

private:
  // methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace agelengthkeys */
} /* namespace niwa */

#endif /* SOURCE_AGELENGTHKEYS_FACTORY_H_ */
