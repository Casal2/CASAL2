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
#ifndef DERIVEDQUANTITIES_AGE_BIOMASS_H_
#define DERIVEDQUANTITIES_AGE_BIOMASS_H_

// headers
#include "../../AgeWeights/AgeWeight.h"
#include "../../DerivedQuantities/DerivedQuantity.h"

// namespaces
namespace niwa {
namespace derivedquantities {
namespace age {

// classes
class Biomass : public niwa::DerivedQuantity {
public:
  // methods
  explicit Biomass(shared_ptr<Model> model);
  virtual ~Biomass() = default;
  void PreExecute() override final;
  void Execute() override final;
  void DoValidate() override final;
  void DoBuild() override final;

protected:
  vector<string>     age_weight_labels_;
  vector<AgeWeight*> age_weights_;
  bool               use_age_weights_ = false;
};

} /* namespace age */
} /* namespace derivedquantities */
} /* namespace niwa */
#endif /* DERIVEDQUANTITIES_BIOMASS_H_ */
