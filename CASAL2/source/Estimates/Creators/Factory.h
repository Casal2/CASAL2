/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 22/09/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef ESTIMATES_CREATORS_FACTORY_H_
#define ESTIMATES_CREATORS_FACTORY_H_

// headers
#include "../../Estimates/Creators/Creator.h"

// namespaces
namespace niwa {
class Model;

namespace estimates {
namespace creators {

/**
 *
 */
class Factory {
public:
  // methods
  static Creator*             Create(shared_ptr<Model> model, const string& object_type, const string& sub_type);

private:
  // methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace creators */
} /* namespace estimates */
} /* namespace niwa */

#endif /* ESTIMATES_CREATORS_FACTORY_H_ */
