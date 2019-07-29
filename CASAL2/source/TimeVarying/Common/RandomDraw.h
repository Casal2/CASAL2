/**
 * @file RandomWalk.h
 * @author Craig Marsh
 * @github https://github.com/Zaita
 * @date 2/02/2016
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This child will add a deviate drawn from a normal distribution to the existing parameter value
 */
#ifndef TIMEVARYING_RANDOM_DRAW_H_
#define TIMEVARYING_RANDOM_DRAW_H_

// headers
#include "TimeVarying/TimeVarying.h"

// namespaces
namespace niwa {
class Estimate;
namespace timevarying {

/**
 * Class definition
 */
class RandomDraw : public TimeVarying {
public:
  explicit RandomDraw(Model* model);
  virtual                     ~RandomDraw() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoUpdate() override final;

private:
  // members
  Double                      mu_;
  Double                      sigma_;
  Double                      intercept_;
  bool                        has_at_estimate_ = false;
  string                      distribution_;
  Double*                     value_;
  Double                      lower_bound_;
  Double                      upper_bound_;

};

} /* namespace timevarying */
} /* namespace niwa */

#endif /* TIMEVARYING_RANDOM_DRAW_H_ */
