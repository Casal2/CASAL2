/**
 * @file Exponential.h
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 */
#ifndef GROWTHINCREMENTS_EXPONENTIAL_H_
#define GROWTHINCREMENTS_EXPONENTIAL_H_

// headers
#include "../../GrowthIncrements/GrowthIncrement.h"

// namespaces
namespace niwa {
namespace growthincrements {

/**
 * class definition
 */
class Exponential : public niwa::GrowthIncrement {
public:
  // methods
  explicit Exponential(shared_ptr<Model> model);
  virtual ~Exponential() = default;
  void DoValidate() override final;
  void DoBuild() override final{};
  void DoReset() override final{};

  // accessors
protected:
  Double get_mean_increment(double length, double time_step_proportion) override;
  Double        g_a_;
  Double        g_b_;
  Double        l_a_;
  Double        l_b_;
};

} /* namespace growthincrements */
} /* namespace niwa */
#endif /* GROWTHINCREMENTS_EXPONENTIAL_H_ */
