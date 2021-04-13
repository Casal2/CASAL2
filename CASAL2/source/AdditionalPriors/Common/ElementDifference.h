/**
 * @file ElementDifference.h
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 28/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * Applied to two vector/map parameters. Square of (vector1[i]-vector2[i]). Encourages the ith elements of the two parameters to be equal.
 */
#ifndef ELEMENT_DIFFERENCE_H_
#define ELEMENT_DIFFERENCE_H_

// headers
#include "../../AdditionalPriors/AdditionalPrior.h"
#include "../../Estimates/Estimate.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * Class definition
 */
class ElementDifference : public niwa::AdditionalPrior {
public:
  // methods
  ElementDifference(shared_ptr<Model> model);
  virtual                     ~ElementDifference() = default;
  Double                      GetScore() final;

protected:
  // methods
  void                        DoValidate() final;
  void                        DoBuild() final;
  // members
  string                      second_parameter_;
  Double*                     addressable_ = nullptr;
  map<unsigned, Double>*      addressable_map_ = nullptr;
  vector<Double>*             addressable_vector_ = nullptr;
  vector<Double*>*            addressable_ptr_vector_ = nullptr;
  Double*                     second_addressable_ = nullptr;
  map<unsigned, Double>*      second_addressable_map_ = nullptr;
  vector<Double>*             second_addressable_vector_ = nullptr;
  vector<Double*>*            second_addressable_ptr_vector_ = nullptr;
  Double                      multiplier_ = 0.0;
};

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ELEMENT_DIFFERENCE_H_ */
