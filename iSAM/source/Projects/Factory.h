/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef PROJECTS_FACTORY_H_
#define PROJECTS_FACTORY_H_

// headers
#include "Projects/Project.h"

// namespaces
namespace niwa {
namespace projects {

/**
 * Class definition
 */
class Factory {
public:
  // methods
  static ProjectPtr Create(const string& object_type, const string& sub_type);

private:
  // methods
  Factory() = delete;
  virtual                     ~Factory() = delete;
};

} /* namespace projects */
} /* namespace niwa */

#endif /* FACTORY_H_ */
