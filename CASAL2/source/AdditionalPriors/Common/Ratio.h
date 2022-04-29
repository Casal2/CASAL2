/**
 * @file Ratio.h
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022  www.niwa.co.nz
 *
 * @section DESCRIPTION
 * prior on the ratio of q1/q2
 */
#ifndef RATIO_H_
#define RATIO_H_

// headers
#include "../../AdditionalPriors/AdditionalPrior.h"
#include "../../Estimates/Estimate.h"

// namespaces
namespace niwa {
namespace additionalpriors {

/**
 * Class definition
 */
class Ratio : public niwa::AdditionalPrior {
public:
  // methods
  Ratio(shared_ptr<Model> model);
  virtual ~Ratio() = default;
  Double GetScore() final;

protected:
  // methods
  void DoValidate() final;
  void DoBuild() final;
  // members
  Double*                addressable_                   = nullptr;
  Double*                second_addressable_            = nullptr;
  string                 second_parameter_;
  Double                 mu_;
  Double                 cv_;
  Double                 sigma_;
};

} /* namespace additionalpriors */
} /* namespace niwa */

#endif /* ELEMENT_DIFFERENCE_H_ */
