/**
 * @file BiomassIndex.h
 * @author  A Dunn
 * @date 01/04/2024
 * @section LICENSE
 *
 * @section DESCRIPTION
 *
 * This derived quantity will calculate the amount of BiomassIndex
 * in the partition with a selectivity
 */
#ifndef DERIVEDQUANTITIES_AGE_BIOMASS_INDEX_H_
#define DERIVEDQUANTITIES_AGE_BIOMASS_INDEX_H_

// headers
#include "../../AgeWeights/AgeWeight.h"
#include "../../Catchabilities/Common/Nuisance.h"
#include "../../DerivedQuantities/DerivedQuantity.h"
#include "../../Model/Model.h"

// namespaces
namespace niwa {
namespace derivedquantities {
namespace age {

using catchabilities::Nuisance;

// classes
class BiomassIndex : public niwa::DerivedQuantity {
public:
  // methods
  explicit BiomassIndex(shared_ptr<Model> model);
  virtual ~BiomassIndex() = default;
  void PreExecute() override final;
  void Execute() override final;
  void DoValidate() override final;
  void DoBuild() override final;

protected:
  vector<string>     age_weight_labels_;
  vector<AgeWeight*> age_weights_;
  bool               use_age_weights_ = false;
  string             distribution_;
  string             catchability_label_;
  Double             catchability_value_    = 1.0;
  Catchability*      catchability_          = nullptr;
  Nuisance*          nuisance_catchability_ = nullptr;
  bool               nuisance_q_            = false;
  Double             biomass_;
  Double             last_biomass_;
  double             cv_    = 0;
  double             sigma_ = 0;
  double             bias_  = 0;
  double             rho_   = 0;
};

} /* namespace age */
} /* namespace derivedquantities */
} /* namespace niwa */
#endif /* DERIVEDQUANTITIES_BIOMASS_INDEX_H_ */
