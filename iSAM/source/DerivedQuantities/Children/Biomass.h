/**
 * @file Biomass.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 18/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef DERIVEDQUANTITIES_BIOMASS_H_
#define DERIVEDQUANTITIES_BIOMASS_H_

// headers
#include "DerivedQuantities/DerivedQuantity.h"

// namespaces
namespace niwa {
namespace derivedquantities {

/**
 * class definition
 */
class Biomass : public niwa::DerivedQuantity {
public:
  // methods
  Biomass() = default;
  virtual                     ~Biomass() = default;
  void                        Execute() override final;
//  void                        Calculate() override final;
//  void                        Calculate(unsigned initialisation_phase) override final;
};

} /* namespace derivedquantities */
} /* namespace niwa */
#endif /* DERIVEDQUANTITIES_BIOMASS_H_ */
