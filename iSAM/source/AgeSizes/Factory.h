/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifndef AGESIZES_FACTORY_H_
#define AGESIZES_FACTORY_H_

// headers
#include "AgeSizes/AgeSize.h"

// namespaces
namespace isam {
namespace agesizes {

/**
 * class definition
 */
class Factory {
public:
  static AgeSizePtr           Create(const string& object_type, const string& sub_type);

private:
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace agesizes */
} /* namespace isam */
#endif /* FACTORY_H_ */
