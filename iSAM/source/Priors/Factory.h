/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 6/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef PRIORS_FACTORY_H_
#define PRIORS_FACTORY_H_

// Headers
#include "Priors/Prior.h"

// Namespaces
namespace isam {
namespace priors {

/**
 * Class Definition
 */
class Factory {
public:
  // methods
  static PriorPtr Create(const string& object_type, const string& sub_type);

private:
  // Methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace priors */
} /* namespace isam */
#endif /* PRIORS_FACTORY_H_ */
