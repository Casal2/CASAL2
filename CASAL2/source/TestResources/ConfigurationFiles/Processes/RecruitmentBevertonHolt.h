/**
 * @file BevertonHolt.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 20/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE
#ifndef TESTRESOURCES_CONFIGURATION_PROCESSES_BEVERTONHOLT_H_
#define TESTRESOURCES_CONFIGURATION_PROCESSES_BEVERTONHOLT_H_

#include <string>

const std::string processes_recruitment_beverton_holt = R"(
@recruitment recruitment
type beverton_holt
categories immature
proportions 1.0
r0 100
b0 init_phase_one
age 1
ssb ssb
ssb_offset 1
standardise_years 1995 1996 1997 1998 1999 2000 2001 2002 2003 2004 2005 2006 2007
recruitment_multipliers 1.0 0.5 0.6 0.4 1.1 1.2 1.2 1.2 1.2 1.2 1.2 1.2 1.0 1.0 1.0
steepness 0.75
)";

#endif /* BEVERTONHOLT_H_ */
#endif /* TESTMODE */
