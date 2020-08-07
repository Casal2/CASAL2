/**
 * @file CasalComplex1.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 3/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section
 *
 * This is testing CASAL2 vs CASAL-Complex-1 test case
 * developed by Alistair Dunn to ensure consistency between
 * CASAL2, CASAL and SPM.
 */
#ifndef TESTCASES_MODELS_CASALCOMPLEX1_H_
#define TESTCASES_MODELS_CASALCOMPLEX1_H_
#ifdef TESTMODE

// headers
#include <string>

// namespaces
namespace niwa {
namespace testresources {
namespace models {

/**
 *
 */
const std::string test_cases_models_casal_complex_1 =
R"(
@model
min_age 2
max_age 25
age_plus t
base_weight_units kgs
start_year 1975
final_year 2002
projection_final_year 2012
initialisation_phases phase1
time_steps one two three

@categories
format sex
names male female
age_lengths age_size_male age_size_female

@initialisation_phase phase1
years 200
exclude_processes fishing

@time_step one
processes halfm fishing halfm

@time_step two
processes recruitment

@time_step three
processes ageing

@derived_quantity ssb
type biomass
categories male female
selectivities male_maturity female_maturity
time_step_proportion 1.0
time_step one

@ageing ageing
categories male female

@recruitment recruitment
type beverton_holt
categories male female
proportions 0.5 0.5
r0 5e6
age 2
steepness 0.9
ycs_values 0.6 0.645 1.345 0.932 1.325 0.945 1.58 1.345 0.946 0.234 1.435 1.0034 1.532 1.014 0.934 0.352 0.643 0.564 1.345 1.865 1.0487 1.0153 0.435 1.325 1.345 1.5434 1.00 1.00
ssb ssb
ycs_years 1973:2000
standardise_ycs_years 1973:1999

@mortality halfm
type constant_rate
categories male female
relative_m_by_age [type=constant; c=1] halfm.one
m 0.10 0.10

@mortality fishing
type event_biomass
categories male female
years 1975:2002
catches 1191 1488 1288 2004 609 750 997 596 302 344 544 362 509 574 804 977 991 2454 2775 2898 4094 3760 3761 3673 3524 3700 3700 3700
U_max 0.9
selectivities chatFselMale chatFselFemale
penalty event_mortality_penalty

@selectivity male_maturity
type logistic
a50 5
ato95 2

@selectivity female_maturity
type logistic
a50 5
ato95 2

@selectivity one
type constant
c 1

@age_length age_size_male
type von_bertalanffy
length_weight [type=none]
k 0.277
t0 0.11
linf 90.3

@age_length age_size_female
type von_bertalanffy
length_weight [type=none]
k 0.202
t0 -0.20
linf 113.4

@penalty event_mortality_penalty
type process
log_scale True
multiplier 10


@selectivity chatTANselMale
type logistic
a50 9
ato95 4

@selectivity chatTANselFemale
type logistic
a50 9
ato95 4
alpha 0.7

@selectivity chatFselMale
type logistic
a50 9
ato95 4

@selectivity chatFselFemale
type logistic
a50 9
ato95 4
alpha 0.7

@report DQ
type derived_quantity
)";

}
}
}
#endif /* TESTMODE */
#endif /* TESTCASES_MODELS_CASALCOMPLEX1_H_ */
