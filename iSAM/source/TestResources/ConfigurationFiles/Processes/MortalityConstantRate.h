/**
 * @file MortalityConstantRate.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 23/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifndef TESTRESOURCES_CONFIGURATION_MORTALITY_CONSTANT_RATE_H_
#define TESTRESOURCES_CONFIGURATION_MORTALITY_CONSTANT_RATE_H_

#include <string>

const std::string mortality_constant_rate_two_sex = R"(
  @mortality halfM
  type constant_rate
  categories immature.male mature.male immature.female mature.female
  M 0.065 0.065 0.065 0.065
  selectivities One One One One
)";

#endif /* AGEING_H_ */
