/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This is the factory class to create minimisers
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef MINIMISERS_FACTORY_H_
#define MINIMISERS_FACTORY_H_

// Headers
#include "../Minimisers/Minimiser.h"

// Namespaces
namespace niwa {
class Model;

namespace minimisers {

/**
 * Class Definition
 */
class Factory {
public:
  // Methods
  static Minimiser* Create(shared_ptr<Model> model, const string& object_type, const string& sub_type);

private:
  // Methods
  Factory()          = delete;
  virtual ~Factory() = delete;
};

} /* namespace minimisers */
} /* namespace niwa */
#endif /* FACTORY_H_ */
