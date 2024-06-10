/**
 * @file Abundance.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2/09/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

#ifndef TESTRESOURCES_CONFIGURATION_DERIVED_QUANTITIES_ABUNDANCE_H_
#define TESTRESOURCES_CONFIGURATION_DERIVED_QUANTITIES_ABUNDANCE_H_

#include <string>

const std::string derived_quantity_abudance_ssb = R"(
@derived_quantity ssb
type abundance
time_step time_step_one
categories mature
selectivities one
)";

#endif /* TESTRESOURCES_CONFIGURATION_DERIVED_QUANTITIES_ABUNDANCE_H_ */
