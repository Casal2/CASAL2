/**
 * @file Basic.h
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 */
#ifndef GROWTHINCREMENTS_BASIC_H_
#define GROWTHINCREMENTS_BASIC_H_

// headers
#include "../../GrowthIncrements/GrowthIncrement.h"

// namespaces
namespace niwa {
namespace growthincrements {

/**
 * class definition
 */
class Basic : public niwa::GrowthIncrement {
public:
  // methods
  explicit Basic(shared_ptr<Model> model);
  virtual ~Basic() = default;
  void DoValidate() override final;
  void DoBuild() override final{};
  void DoReset() override final{};

  // accessors
  Double get_mean_increment(double length) override final;
private:
  Double        g_a_;
  Double        g_b_;
  Double        l_a_;
  Double        l_b_;
};

} /* namespace growthincrements */
} /* namespace niwa */
#endif /* GROWTHINCREMENTS_BASIC_H_ */
