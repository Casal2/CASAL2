/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/08/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifndef ASSERTS_FACTORY_H_
#define ASSERTS_FACTORY_H_

// Headers
#include <string>

#include "Asserts/Assert.h"

// Namespaces
namespace isam {
namespace asserts {

using std::string;

/**
 * Class definition
 */
class Factory {
public:
  // methods
  static AssertPtr Create(string object_type, string sub_type);

private:
  // methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace asserts */
} /* namespace isam */
#endif /* PROCESSES_FACTORY_H_ */
