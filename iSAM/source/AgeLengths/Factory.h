/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifndef AGELENGTHS_FACTORY_H_
#define AGELENGTHS_FACTORY_H_

// headers
#include "AgeLengths/AgeLength.h"

// namespaces
namespace niwa {
namespace agelengths {

/**
 * class definition
 */
class Factory {
public:
  static AgeLengthPtr           Create(const string& object_type, const string& sub_type);

private:
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace agesizes */
} /* namespace niwa */
#endif /* AGELENGTHS_FACTORY_H_ */
