/**
 * @file Factory.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 6/06/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Standard factory class for the derived quantity
 */
#ifndef DERIVEDQUANTITIES_FACTORY_H_
#define DERIVEDQUANTITIES_FACTORY_H_

// namespaces
#include "DerivedQuantities/DerivedQuantity.h"

// namespaces
namespace isam {
namespace derivedquantities {

/**
 * class definition
 */
class Factory {
public:
  static DerivedQuantityPtr Create(const string& block_type, const string& prior_type);
private:
  Factory() = delete;
  virtual ~Factory() = delete;
};

} /* namespace derivedquantities */
} /* namespace isam */
#endif /* FACTORY_H_ */
