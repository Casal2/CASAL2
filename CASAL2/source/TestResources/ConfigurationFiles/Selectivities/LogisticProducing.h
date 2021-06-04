/**
 * @file LogisticProducing.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 30/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifndef TESTRESOURCES_CONFIGURATION_SELECTIVITIES_LOGISTICPRODUCING_H_
#define TESTRESOURCES_CONFIGURATION_SELECTIVITIES_LOGISTICPRODUCING_H_

#include <string>

const std::string selectivities_logistic_producing_maturation = R"(
@selectivity maturation
type logistic_producing
l 1
h 10
a50 3
ato95 3
)";

#endif /* LOGISTICPRODUCING_H_ */
