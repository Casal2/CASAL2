/**
 * @file DoubleNormalPlateau.h
 * @author  C.Marsh
 * @version 1.0
 * @date 2020
 * @section LICENSE
 *
 * Copyright NIWA Science 2020 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef DOUBLENORMALPLATEAU_H_
#define DOUBLENORMALPLATEAU_H_

// Headers
#include "../../Selectivities/Selectivity.h"

// Namespaces
namespace niwa {
namespace selectivities {

/**
 * Class definition
 */
class DoubleNormalPlateau : public niwa::Selectivity {
public:
  // Methods
  explicit DoubleNormalPlateau(shared_ptr<Model> model);
  virtual ~DoubleNormalPlateau() = default;
  void DoValidate() override final;

  Double get_value(Double value)  override final; 
  Double get_value(unsigned value)  override final; 

private:
  // Members
  Double a1_;
  Double a2_;
  Double sigma_l_;
  Double sigma_r_;
  Double alpha_;
};

} /* namespace selectivities */
} /* namespace niwa */
#endif /* DOUBLENORMALPLATEAU_H_ */
