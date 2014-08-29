/*
 * Abundance.h
 *
 *  Created on: 6/06/2013
 *      Author: Admin
 */

#ifndef DERIVEDQUANTITIES_ABUNDANCE_H_
#define DERIVEDQUANTITIES_ABUNDANCE_H_

// headers
#include "DerivedQuantities/DerivedQuantity.h"

// namespaces
namespace isam {
namespace derivedquantities {

/**
 * class definition
 */
class Abundance : public isam::DerivedQuantity {
public:
  // methods
  Abundance() = default;
  virtual                     ~Abundance() = default;
  void                        Execute() override final;
};

} /* namespace derivedquantities */
} /* namespace isam */
#endif /* ABUNDANCE_H_ */
