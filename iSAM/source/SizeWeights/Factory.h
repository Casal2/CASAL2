/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef SIZEWEIGHTS_FACTORY_H_
#define SIZEWEIGHTS_FACTORY_H_

// headers
#include "SizeWeights/SizeWeight.h"

// namespaces
namespace niwa {
namespace sizeweights {

/**
 * class definition
 */
class Factory {
public:
  // methods
  static SizeWeightPtr        Create(const string& object_type, const string& sub_type);

private:
  // methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace sizeweights */
} /* namespace niwa */
#endif /* SIZEWEIGHTS_FACTORY_H_ */
