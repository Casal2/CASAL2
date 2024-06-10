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
 * TODO: Add documentation
 */
#ifndef AGELENGTHS_NONE_H_
#define AGELENGTHS_NONE_H_

// headers
#include "../../AgeLengths/AgeLength.h"

// namespaces
namespace niwa {
namespace agelengths {

/**
 * class definition
 */
class None : public niwa::AgeLength {
public:
  // methods
  explicit None(shared_ptr<Model> model) : AgeLength(model){};
  virtual ~None() = default;
  void DoValidate() override final{};
  void DoBuild() override final{};
  void DoReset() override final{};

  // accessors
  Double calculate_mean_length(unsigned year, unsigned time_step, unsigned age) override final { return 1.0; };

};

} /* namespace agelengths */
} /* namespace niwa */
#endif /* AGELENGTHS_NONE_H_ */
