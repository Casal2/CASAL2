/**
 * @file CasalComplex1.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 3/10/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
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
ycs_values 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00
ssb ssb
ssb_offset 3
standardise_ycs_years 1973:1999

@mortality halfm
type constant_rate
categories male female
selectivities [type=constant; c=1] halfm.one
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

@minimiser gammadiff
type numerical_differences
tolerance 0.000002
iterations 1000
step_size 1e-7
covariance f
active f

@observation chatCPUE
type biomass
catchability [q=6.52606e-005]
time_step one
categories male+female
selectivities chatFselMale chatFselFemale
likelihood lognormal
years 1992:2001
time_step_proportion 1.0
obs 1.50 1.10 0.93 1.33 1.53 0.90 0.68 0.75 0.57 1.23
error_value 0.35

@observation chatTANbiomass
type biomass
catchability [q=0.205139]
time_step one
categories male+female
selectivities chatTANselMale chatTANselFemale
likelihood lognormal
years 1992:2002
time_step_proportion 1.0
obs 4180 2950 3353 3303 2457 2811 2873 2302 2090 1589 1567
error_value 0.15 0.17 0.10 0.23 0.13 0.17 0.18 0.12 0.09 0.13 0.15

@observation chatTANage
type proportions_at_age
likelihood lognormal
time_step one
categories male female
min_age 3
max_age 15
selectivities chatTANselMale chatTANselFemale
years 1992:2004
time_step_proportion 1.0
table obs
1992 0.0186 0.0219 0.0249 0.0390 0.0512 0.0646 0.0422 0.0677 0.0523 0.0687 0.0299 0.0132 0.0515 0.0055 0.0254 0.0199 0.0320 0.0268 0.0394 0.0250 0.0536 0.0346 0.0423 0.0489 0.0304 0.0705
1993 0.0449 0.0346 0.0372 0.0112 0.0286 0.0220 0.0279 0.0156 0.0310 0.0300 0.0690 0.0283 0.0614 0.0552 0.0231 0.0457 0.0301 0.0296 0.0426 0.0122 0.0415 0.0277 0.0541 0.0697 0.0127 0.1142
1994 0.1063 0.0405 0.0431 0.0274 0.0179 0.0182 0.0170 0.0198 0.0317 0.0252 0.0127 0.0195 0.0969 0.1176 0.0483 0.0421 0.0513 0.0271 0.0350 0.0095 0.0154 0.0060 0.0256 0.0083 0.0303 0.1072
1995 0.0933 0.0730 0.0365 0.0123 0.0379 0.0321 0.0195 0.0149 0.0158 0.0314 0.0322 0.0544 0.0329 0.0788 0.0609 0.0501 0.0368 0.0382 0.0288 0.0034 0.0476 0.0218 0.0231 0.0197 0.0390 0.0659
1996 0.0528 0.0505 0.0904 0.0485 0.0226 0.0323 0.0064 0.0149 0.0037 0.0039 0.0046 0.0111 0.0265 0.0689 0.1471 0.1374 0.0786 0.0450 0.0260 0.0135 0.0282 0.0032 0.0168 0.0286 0.0133 0.0251
1997 0.0942 0.0797 0.0590 0.0498 0.0318 0.0528 0.0045 0.0187 0.0091 0.0152 0.0093 0.0230 0.0344 0.1124 0.0682 0.0927 0.0814 0.0225 0.0233 0.0180 0.0073 0.0026 0.0027 0.0297 0.0121 0.0457
1998 0.0397 0.0678 0.0862 0.0457 0.0676 0.0354 0.0201 0.0225 0.0092 0.0176 0.0066 0.0260 0.0422 0.0195 0.0606 0.0660 0.0831 0.0711 0.0527 0.0291 0.0170 0.0362 0.0225 0.0095 0.0049 0.0411
1999 0.0683 0.0771 0.0408 0.0364 0.0228 0.0380 0.0148 0.0226 0.0138 0.0109 0.0045 0.0050 0.0585 0.0628 0.0307 0.0711 0.0411 0.0372 0.0740 0.0521 0.0465 0.0232 0.0270 0.0180 0.0152 0.0876
2000 0.0623 0.0466 0.0521 0.0292 0.0369 0.0524 0.0508 0.0414 0.0385 0.0138 0.0120 0.0227 0.0234 0.0131 0.0358 0.0336 0.0433 0.0445 0.0699 0.0413 0.0265 0.0298 0.0368 0.0187 0.0370 0.0878
2001 0.0033 0.0274 0.0554 0.0259 0.0455 0.0611 0.0413 0.0404 0.0337 0.0204 0.0124 0.0034 0.0195 0.0064 0.0314 0.0278 0.0364 0.0983 0.0549 0.0798 0.0681 0.0728 0.0488 0.0076 0.0210 0.0567
2002 0.0173 0.0193 0.0241 0.0346 0.0365 0.0657 0.0427 0.0667 0.0326 0.0307 0.0272 0.0141 0.0319 0.0353 0.0249 0.0146 0.0133 0.0547 0.0488 0.0745 0.0660 0.0750 0.0646 0.0304 0.0147 0.0399
2003 0.1079 0.0696 0.0580 0.0607 0.0865 0.0706 0.0288 0.0247 0.0062 0.0077 0.0076 0.0070 0.0115 0.0905 0.0581 0.0608 0.0373 0.0427 0.0548 0.0241 0.0247 0.0245 0.0105 0.0063 0.0036 0.0152
2004 0.0264 0.0641 0.0445 0.0714 0.0413 0.0516 0.0329 0.0271 0.0270 0.0117 0.0023 0.0021 0.0209 0.0229 0.0690 0.0485 0.0913 0.0563 0.0537 0.0594 0.0517 0.0412 0.0133 0.0157 0.0137 0.0401
end_table
table error_values
1992 0.710 0.469 0.370 0.350 0.419 0.413 0.373 0.383 0.365 0.301 0.393 0.518 0.302 0.689 0.326 0.394 0.313 0.305 0.322 0.336 0.301 0.308 0.289 0.276 0.360 0.228
1993 0.321 0.336 0.361 0.861 0.421 0.532 0.408 0.555 0.409 0.489 0.361 0.527 0.369 0.346 0.451 0.370 0.428 0.437 0.376 0.780 0.336 0.494 0.366 0.342 0.628 0.307
1994 0.268 0.379 0.315 0.492 0.555 0.507 0.587 0.546 0.468 0.510 0.818 0.547 0.268 0.238 0.290 0.315 0.278 0.405 0.382 0.782 0.640 0.838 0.457 0.742 0.374 0.188
1995 0.250 0.367 0.372 0.621 0.655 0.810 0.924 0.772 0.865 0.713 0.632 0.608 0.612 0.254 0.329 0.340 0.400 0.379 0.587 1.028 0.432 0.542 0.499 0.506 0.475 0.286
1996 0.497 0.445 0.353 0.409 0.729 0.560 1.161 0.929 1.481 1.128 1.335 1.228 0.687 0.392 0.227 0.256 0.317 0.381 0.566 0.716 0.542 3.000 0.881 0.568 0.930 0.558
1997 0.298 0.315 0.430 0.441 0.612 0.521 1.504 0.736 0.994 0.867 1.033 0.668 0.607 0.289 0.313 0.279 0.301 0.631 0.518 0.695 1.059 3.000 1.751 0.556 1.051 0.458
1998 0.464 0.403 0.343 0.440 0.423 0.575 0.710 0.703 1.227 0.774 1.503 0.691 0.445 0.533 0.342 0.341 0.324 0.358 0.357 0.548 0.689 0.511 0.689 1.097 1.381 0.493
1999 0.572 0.417 0.454 0.510 0.577 0.521 0.834 0.782 0.808 1.004 1.373 1.170 0.457 0.458 0.550 0.314 0.440 0.448 0.324 0.335 0.450 0.690 0.588 0.813 0.749 0.426
2000 0.414 0.522 0.328 0.399 0.351 0.319 0.320 0.328 0.374 0.631 0.613 0.589 0.419 1.334 0.569 0.452 0.403 0.371 0.294 0.343 0.427 0.488 0.389 0.593 0.416 0.229
2001 1.726 0.527 0.446 0.510 0.510 0.392 0.462 0.442 0.551 0.643 0.761 1.439 0.637 1.180 0.434 0.552 0.445 0.301 0.429 0.352 0.353 0.368 0.441 0.767 0.610 0.362
2002 1.091 0.770 0.539 0.421 0.412 0.297 0.367 0.322 0.391 0.510 0.523 0.734 0.481 0.612 0.643 0.756 0.772 0.399 0.369 0.331 0.306 0.304 0.309 0.461 0.752 0.423
2003 0.175 0.232 0.227 0.207 0.173 0.172 0.278 0.299 0.515 0.487 0.440 0.494 0.307 0.183 0.209 0.228 0.269 0.234 0.184 0.296 0.282 0.295 0.459 0.402 0.746 0.275
2004 0.328 0.221 0.313 0.250 0.269 0.283 0.270 0.361 0.380 0.481 1.162 1.189 0.409 0.332 0.217 0.329 0.196 0.257 0.235 0.222 0.244 0.250 0.421 0.583 0.512 0.259
end_table

@observation chatOBS
type proportions_at_age
likelihood lognormal
time_step one
categories male female
min_age 3
max_age 15
selectivities chatFselMale chatFselFemale
years 2000:2001
time_step_proportion 1.0
table obs
2000 0.0161 0.0441 0.0605 0.0509 0.0658 0.0590 0.0715 0.0432 0.0291 0.0154 0.0116 0.0051 0.0180 0.0129 0.0405 0.0315 0.0428 0.0766 0.1011 0.0573 0.0309 0.0436 0.0248 0.0071 0.0060 0.0346
2001 0.0087 0.0280 0.0422 0.0427 0.0849 0.0887 0.0788 0.0711 0.0566 0.0275 0.0162 0.0166 0.0507 0.0019 0.0383 0.0246 0.0332 0.0786 0.0594 0.0345 0.0295 0.0240 0.0219 0.0120 0.0062 0.0233
end_table
table error_values
2000 0.495 0.264 0.237 0.233 0.196 0.219 0.203 0.238 0.349 0.426 0.458 0.714 0.343 0.424 0.299 0.404 0.252 0.188 0.146 0.208 0.275 0.217 0.294 0.424 0.514 0.184
2001 0.383 0.332 0.311 0.300 0.192 0.200 0.227 0.245 0.268 0.372 0.551 0.602 0.257 1.445 0.257 0.333 0.305 0.191 0.195 0.211 0.255 0.265 0.295 0.356 0.481 0.239
end_table

@estimate
parameter catchability[chatCPUE.one].q
type uniform_log
lower_bound 1e-8
upper_bound 10

@estimate
parameter catchability[chatTANbiomass.one].q
type uniform_log
lower_bound 1e-2
upper_bound 1

@estimate
parameter process[recruitment].r0
lower_bound 1e3
upper_bound 1e7
type uniform_log

@estimate
parameter selectivity[chatTANselMale].a50
lower_bound 0
upper_bound 30
type uniform

@estimate
parameter selectivity[chatTANselMale].ato95
lower_bound 0
upper_bound 30
type uniform

@estimate
parameter selectivity[chatTANselFemale].a50
lower_bound 0
upper_bound 30
type uniform

@estimate
parameter selectivity[chatTANselFemale].ato95
lower_bound 0
upper_bound 30
type uniform

@estimate
parameter selectivity[chatTANselFemale].alpha
lower_bound 0.2
upper_bound 5
type uniform

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
)";

}
}
}
#endif /* TESTMODE */
#endif /* TESTCASES_MODELS_CASALCOMPLEX1_H_ */
