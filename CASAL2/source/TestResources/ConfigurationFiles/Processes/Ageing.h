/**
 * @file Ageing.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 23/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifndef TESTRESOURCES_CONFIGURATION_PROCESSES_AGEING_H_
#define TESTRESOURCES_CONFIGURATION_PROCESSES_AGEING_H_

#include <string>

const std::string processes_ageing_no_sex = R"(
@ageing ageing
categories immature mature spawning
)";

const std::string processes_ageing_no_sex_no_spawning = R"(
@ageing ageing
categories immature mature
)";

const std::string processes_ageing_two_sex = R"(
@ageing ageing
categories immature.male immature.female mature.male mature.female
)";




#endif /* AGEING_H_ */
