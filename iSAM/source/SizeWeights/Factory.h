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
namespace isam {
namespace sizeweights {

/**
 * class definition
 */
class Factory {
public:
  // methods
  SizeWeightPtr               Create(const string& block_type, const string& object_type);

private:
  // methods
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace sizeweights */
} /* namespace isam */
#endif /* SIZEWEIGHTS_FACTORY_H_ */
