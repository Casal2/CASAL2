/**
 * @file TimeStepProportionsByCategory.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 10/03/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef AGE_OBSERVATIONS_TIMESTEPPROPORTIONSBYCATEGORY_H_
#define AGE_OBSERVATIONS_TIMESTEPPROPORTIONSBYCATEGORY_H_

// headers
#include "Observations/Age/ProportionsByCategory.h"

// namespaces
namespace niwa {
namespace observations {
namespace age {

/**
 *
 */
class TimeStepProportionsByCategory : public observations::age::ProportionsByCategory {
public:
  // methods
  TimeStepProportionsByCategory(shared_ptr<Model> model);
  virtual                     ~TimeStepProportionsByCategory() = default;
  void                        DoBuild() override final;

private:
  Double                      time_step_proportion_;
};

} /* namespace age */
} /* namespace observations */
} /* namespace niwa */

#endif /* AGE_OBSERVATIONS_TIMESTEPPROPORTIONSBYCATEGORY_H_ */
