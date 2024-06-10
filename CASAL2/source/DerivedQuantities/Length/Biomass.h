/**
 * @file Biomass.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 18/07/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This derived quantity will calculate the amount of biomass
 * in the partition with a selectivity
 */
#ifndef DERIVEDQUANTITIES_LENGTH_BIOMASS_H_
#define DERIVEDQUANTITIES_LENGTH_BIOMASS_H_

// headers
#include "../../DerivedQuantities/DerivedQuantity.h"

// namespaces
namespace niwa {
namespace derivedquantities {
namespace length {

// classes
class Biomass : public niwa::DerivedQuantity {
public:
  // methods
  explicit Biomass(shared_ptr<Model> model) : DerivedQuantity(model) {}
  virtual ~Biomass() = default;
  void PreExecute() override final;
  void Execute() override final;

  void DoValidate() override final{};
  void DoBuild() override final{};
};

} /* namespace length */
} /* namespace derivedquantities */
} /* namespace niwa */

#endif /* DERIVEDQUANTITIES_LENGTH_BIOMASS_H_ */
