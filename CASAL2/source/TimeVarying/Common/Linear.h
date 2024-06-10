/**
 * @file Linear.h
 * @author Craig Marsh
 * @github https://github.com/Zaita
 * @date 2/02/2016
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/�2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This child will add a deviate drawn from a normal distribution to the existing parameter value
 */
#ifndef TIMEVARYING_LINEAR_H_
#define TIMEVARYING_LINEAR_H_

// headers
#include "../../TimeVarying/TimeVarying.h"

// namespaces
namespace niwa {
class Estimate;
namespace timevarying {

/**
 * Class definition
 */
class Linear : public TimeVarying {
public:
  explicit Linear(shared_ptr<Model> model);
  virtual ~Linear() = default;
  void DoValidate() override final;
  void DoBuild() override final;
  void DoReset() override final;
  void DoUpdate() override final;

private:
  // members
  Double                slope_;
  Double                intercept_;
  Double*               value_;
  map<unsigned, Double> values_by_year_;
};

} /* namespace timevarying */
} /* namespace niwa */

#endif /* TIMEVARYING_RANDOMWALK_H_ */
