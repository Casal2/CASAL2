/**
 * @file None.h
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
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
  Double get_mean_increment(double length, double time_step_proportion) override final { return 1.0; };

  // accessors


};

} /* namespace growthincrements */
} /* namespace niwa */
#endif /* GROWTHINCREMENTS_NONE_H_ */
