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
  Abundance();
  virtual                     ~Abundance() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final { };
  void                        DoReset() override final { };

private:
  // members
  string                      time_step_label_;
  vector<string>              initialisation_time_step_labels_;
  vector<string>              category_labels_;
  vector<string>              selectivity_labels_;
};

} /* namespace derivedquantities */
} /* namespace isam */
#endif /* ABUNDANCE_H_ */
