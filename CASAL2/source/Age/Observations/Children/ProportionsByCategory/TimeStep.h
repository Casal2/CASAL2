/**
 * @file TimeStepProportionsByCategory.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 10/03/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGE_OBSERVATIONS_TIMESTEPPROPORTIONSBYCATEGORY_H_
#define AGE_OBSERVATIONS_TIMESTEPPROPORTIONSBYCATEGORY_H_

// headers
#include "Age/Observations/Children/ProportionsByCategory.h"

// namespaces
namespace niwa {
namespace age {
namespace observations {

/**
 *
 */
class TimeStepProportionsByCategory : public observations::ProportionsByCategory {
public:
  // methods
  TimeStepProportionsByCategory(Model* model);
  virtual                     ~TimeStepProportionsByCategory() = default;
  void                        DoBuild() override final;

private:
  Double                      time_step_proportion_;
};

} /* namespace observations */
} /* namespace age */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_TIMESTEPPROPORTIONSBYCATEGORY_H_ */
