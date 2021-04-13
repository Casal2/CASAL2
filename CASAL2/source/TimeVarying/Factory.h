/**
 * @file Factory.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/01/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef TIMEVARYING_FACTORY_H_
#define TIMEVARYING_FACTORY_H_

// headers
#include "../TimeVarying/TimeVarying.h"

// namespaces
namespace niwa {
class Model;

namespace timevarying {

/**
 * Class definition
 */
class Factory {
public:
  // methods
  static TimeVarying* Create(shared_ptr<Model> model, const string& object_type, const string& sub_type);

private:
  // methods
  Factory() = delete;
  virtual                     ~Factory() = delete;
};

} /* namespace timevarying */
} /* namespace niwa */

#endif /* TIMEVARYING_FACTORY_H_ */
