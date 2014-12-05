/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef FACTORY_H_
#define FACTORY_H_

// Headers
#include "BaseClasses/Factory.h"
#include "Catchabilities/Manager.h"

// Namespaces
namespace niwa {
namespace catchabilities {

/**
 * Class definition
 */
class Factory {
public:
  // methods
  static CatchabilityPtr Create(const string& object_type, const string& sub_type);

private:
  Factory() = delete;
  ~Factory() = delete;
};

} /* namespace catchabilities */
} /* namespace niwa */
#endif /* FACTORY_H_ */
