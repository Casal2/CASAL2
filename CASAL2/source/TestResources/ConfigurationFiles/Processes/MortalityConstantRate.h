/**
 * @file MortalityConstantRate.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 23/08/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */
#ifndef TESTRESOURCES_CONFIGURATION_MORTALITY_CONSTANT_RATE_H_
#define TESTRESOURCES_CONFIGURATION_MORTALITY_CONSTANT_RATE_H_

#include <string>

const std::string processes_mortality_constant_rate_two_sex = R"(
@mortality halfM
type constant_rate
categories immature.male mature.male immature.female mature.female
M 0.065 0.065 0.065 0.065
selectivities One One One One
time_step_proportions 1.0
)";

const std::string processes_mortality_constant_rate_no_sex = R"(
@mortality mortality
type constant_rate
categories immature mature
m 0.15 0.15
selectivities one one
time_step_proportions 1.0
)";

#endif /* AGEING_H_ */
