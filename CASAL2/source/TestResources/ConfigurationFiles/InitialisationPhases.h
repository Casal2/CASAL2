/**
 * @file InitialisationPhases.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 30/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifndef TESTFIXTURES_CONFIGURATION_INITIALISATIONPHASES_H_
#define TESTFIXTURES_CONFIGURATION_INITIALISATIONPHASES_H_

#include <string>

const std::string initialisation_phases_one = R"(
@initialisation_phase init_phase_one
processes ageing recruitment maturation mortality
years 200
)";


#endif /* INITIALISATIONPHASES_H_ */
