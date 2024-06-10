/**
 * @file None.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 24/07/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef LENGTHWEIGHTS_NONE_H_
#define LENGTHWEIGHTS_NONE_H_

// headers
#include "../../LengthWeights/LengthWeight.h"

// namespaces
namespace niwa {
namespace lengthweights {

/**
 * class definition
 */
class None : public niwa::LengthWeight {
public:
  // methods
  explicit None(shared_ptr<Model> model) : LengthWeight(model){};
  virtual ~None() = default;
  void DoValidate() override final{};
  void DoBuild() override final{};
  void DoReset() override final{};

  // accessors
  Double mean_weight(Double size, Distribution distribution, Double cv) const override final { return 1.0; }
  Double mean_weight(Double size) const override final { return 1.0; }

  string weight_units() const override final { return ""; };

};

} /* namespace lengthweights */
} /* namespace niwa */
#endif /* LENGTHWEIGHTS_NONE_H_ */
