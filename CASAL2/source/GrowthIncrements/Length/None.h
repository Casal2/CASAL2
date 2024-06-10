/**
 * @file None.h
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 */
#ifndef GROWTHINCREMENTS_NONE_H_
#define GROWTHINCREMENTS_NONE_H_

// headers
#include "../../GrowthIncrements/GrowthIncrement.h"

// namespaces
namespace niwa {
namespace growthincrements {

/**
 * class definition
 */
class None : public niwa::GrowthIncrement {
public:
  // methods
  explicit None(shared_ptr<Model> model) : GrowthIncrement(model){};
  virtual ~None() = default;
  void DoValidate() override final{};
  void DoBuild() override final{};
  void DoReset() override final{};

  // accessors
protected:
  Double get_mean_increment(double length) override final { return 0.0; };

  // accessors
};

} /* namespace growthincrements */
} /* namespace niwa */
#endif /* GROWTHINCREMENTS_NONE_H_ */
