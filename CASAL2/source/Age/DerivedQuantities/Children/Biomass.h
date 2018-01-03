/**
 * @file Biomass.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 18/07/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This derived quantity will calculate the amount of biomass
 * in the partition with a selectivity
 */
#ifndef DERIVEDQUANTITIES_AGE_BIOMASS_H_
#define DERIVEDQUANTITIES_AGE_BIOMASS_H_

// headers
#include "Common/DerivedQuantities/DerivedQuantity.h"
#include "Age/AgeWeights/AgeWeight.h"

// namespaces
namespace niwa {
namespace age {
namespace derivedquantities {

// classes
class Biomass : public niwa::DerivedQuantity {
public:
  // methods
  explicit Biomass(Model* model);
  virtual                     ~Biomass() = default;
  void                        PreExecute() override final;
  void                        Execute() override final;
  void                        DoValidate() override final;
  void                        DoBuild() override final;

protected:
  vector<string>              age_weight_labels_;
  vector<AgeWeight*>          age_weights_;
};

} /* namespace derivedquantities */
} /* namespace age */
} /* namespace niwa */
#endif /* DERIVEDQUANTITIES_BIOMASS_H_ */
