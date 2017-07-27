/**
 * @file Exogenous.h
 * @author C.Marsh
 * @github https://github.com/Zaita
 * @date 29/09/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 * This class allows a parameter to vary by year, based on a known exogenous variable and a shift parameter that
 * can be estimated.
 */
#ifndef TIMEVARYING_EXOGENEOUS_H_
#define TIMEVARYING_EXOGENEOUS_H_

// headers
#include "Common/TimeVarying/TimeVarying.h"

// namespaces
namespace niwa {
namespace timevarying {

/**
 * Class definition
 */
class Exogenous : public TimeVarying {
public:
  explicit Exogenous(Model* model);
  virtual                     ~Exogenous() = default;
  void                        DoValidate() override final;
  void                        DoBuild() override final;
  void                        DoReset() override final;
  void                        DoUpdate() override final;

private:
  // members
  vector<Double>              exogenous_;
  Double                      a_;
  map<unsigned, Double>       values_by_year_;
};

} /* namespace timevarying */
} /* namespace niwa */

#endif /* TIMEVARYING_EXOGENEOUS_H_ */
