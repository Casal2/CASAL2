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
#ifndef TIMEVARYING_RANDOMWALK_H_
#define TIMEVARYING_RANDOMWALK_H_

// headers
#include "TimeVarying/TimeVarying.h"

// namespaces
namespace niwa {
class Estimate;
namespace timevarying {

/**
 * Class definition
 */
class RandomWalk : public TimeVarying {
public:
  explicit RandomWalk(Model* model);
  virtual                     ~RandomWalk() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoUpdate() override final;

private:
  // members
  Double                      mu_;
  Double                      sigma_;
  string                      distribution_;
  Double*                     value_;
  map<unsigned, Double>       values_by_year_;
  Double                      lower_bound_;
  Double                      upper_bound_;

};

} /* namespace timevarying */
} /* namespace niwa */

#endif /* TIMEVARYING_RANDOMWALK_H_ */
