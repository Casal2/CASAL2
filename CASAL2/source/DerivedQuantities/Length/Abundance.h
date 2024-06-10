/**
 * @file Abundance.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 24/06/2015
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * The abundance derived quantity will get the amount
 * of population in the partition with selectivity
 */
#ifndef DERIVEDQUANTITIES_LENGTH_ABUNDANCE_H_
#define DERIVEDQUANTITIES_LENGTH_ABUNDANCE_H_

// headers
#include "../../DerivedQuantities/DerivedQuantity.h"

// namespaces
namespace niwa {
namespace derivedquantities {
namespace length {

// classes
class Abundance : public niwa::DerivedQuantity {
public:
  // methods
  explicit Abundance(shared_ptr<Model> model) : DerivedQuantity(model) {}
  virtual ~Abundance() = default;
  void PreExecute() override final;
  void Execute() override final;
  void DoValidate() override final{};
  void DoBuild() override final{};
};

} /* namespace length */
} /* namespace derivedquantities */
} /* namespace niwa */
#endif /* DERIVEDQUANTITIES_LENGTH_ABUNDANCE_H_ */
