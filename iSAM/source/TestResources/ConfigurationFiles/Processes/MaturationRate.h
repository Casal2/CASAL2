/**
 * @file MaturationRate.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 30/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifndef TESTRESOURCES_CONFIGURATION_MATURATIONRATE_H_
#define TESTRESOURCES_CONFIGURATION_MATURATIONRATE_H_

#include <string>

const std::string processes_maturation_rate_immature_mature = R"(
@maturation maturation
type rate
from immature
to mature
proportions 1.0
selectivities maturation
)";


#endif /* MATURATIONRATE_H_ */
