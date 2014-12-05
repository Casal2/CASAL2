/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 23/01/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef SELECTIVITIES_FACTORY_H_
#define SELECTIVITIES_FACTORY_H_

// Headers
#include "BaseClasses/Factory.h"
#include "Selectivities/Selectivity.h"
#include "Selectivities/Manager.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Class Definition
 */
class Factory {
public:
  // methods
  static SelectivityPtr Create(const string& object_type, const string& sub_type);

private:
  // methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* SELECTIVITIES_FACTORY_H_ */
