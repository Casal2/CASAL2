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
#ifndef OBSERVATIONS_TIMESTEPPROPORTIONSBYCATEGORY_H_
#define OBSERVATIONS_TIMESTEPPROPORTIONSBYCATEGORY_H_

// headers
#include <Observations/Children/ProportionsByCategory/ProportionsByCategory.h>

// namespaces
namespace niwa {
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
} /* namespace niwa */

#endif /* OBSERVATIONS_TIMESTEPPROPORTIONSBYCATEGORY_H_ */
