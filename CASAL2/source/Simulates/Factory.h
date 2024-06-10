/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 28/05/2014
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SIMULATES_FACTORY_H_
#define SIMULATES_FACTORY_H_

// headers
#include "../Simulates/Simulate.h"

// namespaces
namespace niwa {
class Model;

namespace simulates {

/**
 * Class definition
 */
class Factory {
public:
  // methods
  static Simulate* Create(shared_ptr<Model> model, const string& object_type, const string& sub_type);

private:
  // methods
  Factory()          = delete;
  virtual ~Factory() = delete;
};

} /* namespace simulates */
} /* namespace niwa */

#endif /* FACTORY_H_ */
