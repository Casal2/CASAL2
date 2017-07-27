/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/03/2013
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
#ifndef LIKELIHOODS_FACTORY_H_
#define LIKELIHOODS_FACTORY_H_

// Headers
#include "Common/Likelihoods/Likelihood.h"

// Namespaces
namespace niwa {
class Model;
namespace likelihoods {

/**
 * Class Definition
 */
class Factory {
public:
  // methods
  static Likelihood*          Create(Model* model, const string& object_type, const string& sub_type);

private:
  // Methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace likelihoods */
} /* namespace niwa */
#endif /* LIKELIHOODS_FACTORY_H_ */
