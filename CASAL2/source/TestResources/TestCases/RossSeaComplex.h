/**
 * @file RossSeaComplex.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 10/10/2018
 * @section LICENSE
 *
 * Copyright NIWA Science �2018 - www.niwa.co.nz
 *
 * @section
 *
 * This is testing a Complex Casal2 model with instant mortality
 */
#ifndef SOURCE_TESTRESOURCES_MODELS_ROSSSEACOMPLEX_H_
#define SOURCE_TESTRESOURCES_MODELS_ROSSSEACOMPLEX_H_
#ifdef TESTMODE

// headers
#include <string>

// namespaces
namespace niwa {
namespace testcases {

/**
 * Test case based roughly on the Ross Sea using generic data
 */
const std::string test_cases_models_casal_ross_sea_complex_with_partition_asserts =
    R"(
@model
min_age    1
max_age   50
age_plus T
start_year   1995
final_year   2015
projection_final_year 2051
base_weight_units tonnes
initialisation_phases phase1
time_steps Summer Winter Winter_age
length_bins 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220 230
length_plus False

@categories
format sex.tagyear
names male,female.untagged,2001:2014
age_lengths age_length_male_untagged age_length_male_tagged*14 age_length_female_untagged*1 age_length_female_tagged*14

@initialisation_phase phase1
type derived

@time_step Summer
processes Recruitment Mortality 2001Tags_shelf 2002Tags_shelf 2003Tags_shelf 2004Tags_shelf 2005Tags_shelf 2006Tags_shelf 2007Tags_shelf 2008Tags_shelf 2009Tags_shelf 2010Tags_shelf 2011Tags_shelf 2012Tags_shelf 2013Tags_shelf 2014Tags_shelf 2001Tags_slope 2002Tags_slope 2003Tags_slope 2004Tags_slope 2005Tags_slope 2006Tags_slope 2007Tags_slope 2008Tags_slope 2009Tags_slope 2010Tags_slope 2011Tags_slope 2012Tags_slope 2013Tags_slope 2014Tags_slope 2001Tags_north 2002Tags_north 2003Tags_north 2004Tags_north 2005Tags_north 2006Tags_north 2007Tags_north 2008Tags_north 2009Tags_north 2010Tags_north 2011Tags_north 2012Tags_north 2013Tags_north 2014Tags_north

@time_step Winter
processes  Mortality Tag_loss_0102 Tag_loss_03 Tag_loss_recent

@time_step Winter_age
processes  Ageing

@process Ageing
type ageing
categories *

@catchability surveyBiomassq
type free
q 0.00609399

@process Recruitment
type recruitment_beverton_holt
categories male,female.untagged,2001:2014
proportions 0.5 0*14 0.5 0*14
b0 80000
standardise_ycs_years 2003:2009
ycs_values 1*21
steepness 0.75
ssb SSB
age 1
ycs_years 1994:2014

@derived_quantity SSB
type biomass
time_step Winter
categories *
time_step_proportion 0.5
time_step_proportion_method weighted_sum
selectivities MaturityMale*15 MaturityFemale*15

@process Mortality
type mortality_instantaneous
m 0.13*30
relative_m_by_age One*30
categories *
time_step_proportions 0.5 0.5
table catches
year FishingShelf FishingSlope FishingNorth
1998  8.030 28.970    3.970
1999  14.310  281.500   0.430
2000  64.370  687.540   0
2001  112.540 347.070   132.500
2002  10.100  933.220   411.960
2003  2.290 608.650   1157.940
2004  140.560 1667.390  369.770
2005  397.320 2262.320  550.090
2006  250.840 2372.830  343.300
2007  67.530  2438.330  573.010
2008  60.550  1938.910  250.950
2009  134.500 1904.160  392.850
2010  327.910 2170.510  370.000
2011  483.230 2053.800  310.360
2012  276.810 2375.220  546.840
2013  268.450 2506.030  411.340
2014  265.300 2314.770  340.580
2015  232.620 2236.180  364.140
end_table
table method
method  category  selectivity u_max time_step penalty
FishingShelf  male.untagged ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  male.2001 ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  male.2002 ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  male.2003 ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  male.2004 ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  male.2005 ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  male.2006 ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  male.2007 ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  male.2008 ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  male.2009 ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  male.2010 ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  male.2011 ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  male.2012 ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  male.2013 ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  male.2014 ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  female.untagged ShelfselFemale  0.99  Summer  event_mortality_penalty
FishingShelf  female.2001 ShelfselFemale  0.99  Summer  event_mortality_penalty
FishingShelf  female.2002 ShelfselFemale  0.99  Summer  event_mortality_penalty
FishingShelf  female.2003 ShelfselFemale  0.99  Summer  event_mortality_penalty
FishingShelf  female.2004 ShelfselFemale  0.99  Summer  event_mortality_penalty
FishingShelf  female.2005 ShelfselFemale  0.99  Summer  event_mortality_penalty
FishingShelf  female.2006 ShelfselFemale  0.99  Summer  event_mortality_penalty
FishingShelf  female.2007 ShelfselFemale  0.99  Summer  event_mortality_penalty
FishingShelf  female.2008 ShelfselFemale  0.99  Summer  event_mortality_penalty
FishingShelf  female.2009 ShelfselFemale  0.99  Summer  event_mortality_penalty
FishingShelf  female.2010 ShelfselFemale  0.99  Summer  event_mortality_penalty
FishingShelf  female.2011 ShelfselFemale  0.99  Summer  event_mortality_penalty
FishingShelf  female.2012 ShelfselFemale  0.99  Summer  event_mortality_penalty
FishingShelf  female.2013 ShelfselFemale  0.99  Summer  event_mortality_penalty
FishingShelf  female.2014 ShelfselFemale  0.99  Summer  event_mortality_penalty
FishingSlope  male.untagged SlopeselMale  0.99  Summer  event_mortality_penalty
FishingSlope  male.2001 SlopeselMale  0.99  Summer  event_mortality_penalty
FishingSlope  male.2002 SlopeselMale  0.99  Summer  event_mortality_penalty
FishingSlope  male.2003 SlopeselMale  0.99  Summer  event_mortality_penalty
FishingSlope  male.2004 SlopeselMale  0.99  Summer  event_mortality_penalty
FishingSlope  male.2005 SlopeselMale  0.99  Summer  event_mortality_penalty
FishingSlope  male.2006 SlopeselMale  0.99  Summer  event_mortality_penalty
FishingSlope  male.2007 SlopeselMale  0.99  Summer  event_mortality_penalty
FishingSlope  male.2008 SlopeselMale  0.99  Summer  event_mortality_penalty
FishingSlope  male.2009 SlopeselMale  0.99  Summer  event_mortality_penalty
FishingSlope  male.2010 SlopeselMale  0.99  Summer  event_mortality_penalty
FishingSlope  male.2011 SlopeselMale  0.99  Summer  event_mortality_penalty
FishingSlope  male.2012 SlopeselMale  0.99  Summer  event_mortality_penalty
FishingSlope  male.2013 SlopeselMale  0.99  Summer  event_mortality_penalty
FishingSlope  male.2014 SlopeselMale  0.99  Summer  event_mortality_penalty
FishingSlope  female.untagged SlopeselFemale  0.99  Summer  event_mortality_penalty
FishingSlope  female.2001 SlopeselFemale  0.99  Summer  event_mortality_penalty
FishingSlope  female.2002 SlopeselFemale  0.99  Summer  event_mortality_penalty
FishingSlope  female.2003 SlopeselFemale  0.99  Summer  event_mortality_penalty
FishingSlope  female.2004 SlopeselFemale  0.99  Summer  event_mortality_penalty
FishingSlope  female.2005 SlopeselFemale  0.99  Summer  event_mortality_penalty
FishingSlope  female.2006 SlopeselFemale  0.99  Summer  event_mortality_penalty
FishingSlope  female.2007 SlopeselFemale  0.99  Summer  event_mortality_penalty
FishingSlope  female.2008 SlopeselFemale  0.99  Summer  event_mortality_penalty
FishingSlope  female.2009 SlopeselFemale  0.99  Summer  event_mortality_penalty
FishingSlope  female.2010 SlopeselFemale  0.99  Summer  event_mortality_penalty
FishingSlope  female.2011 SlopeselFemale  0.99  Summer  event_mortality_penalty
FishingSlope  female.2012 SlopeselFemale  0.99  Summer  event_mortality_penalty
FishingSlope  female.2013 SlopeselFemale  0.99  Summer  event_mortality_penalty
FishingSlope  female.2014 SlopeselFemale  0.99  Summer  event_mortality_penalty
FishingNorth  male.untagged NorthselMale  0.99  Summer  event_mortality_penalty
FishingNorth  male.2001 NorthselMale  0.99  Summer  event_mortality_penalty
FishingNorth  male.2002 NorthselMale  0.99  Summer  event_mortality_penalty
FishingNorth  male.2003 NorthselMale  0.99  Summer  event_mortality_penalty
FishingNorth  male.2004 NorthselMale  0.99  Summer  event_mortality_penalty
FishingNorth  male.2005 NorthselMale  0.99  Summer  event_mortality_penalty
FishingNorth  male.2006 NorthselMale  0.99  Summer  event_mortality_penalty
FishingNorth  male.2007 NorthselMale  0.99  Summer  event_mortality_penalty
FishingNorth  male.2008 NorthselMale  0.99  Summer  event_mortality_penalty
FishingNorth  male.2009 NorthselMale  0.99  Summer  event_mortality_penalty
FishingNorth  male.2010 NorthselMale  0.99  Summer  event_mortality_penalty
FishingNorth  male.2011 NorthselMale  0.99  Summer  event_mortality_penalty
FishingNorth  male.2012 NorthselMale  0.99  Summer  event_mortality_penalty
FishingNorth  male.2013 NorthselMale  0.99  Summer  event_mortality_penalty
FishingNorth  male.2014 NorthselMale  0.99  Summer  event_mortality_penalty
FishingNorth  female.untagged NorthselFemale  0.99  Summer  event_mortality_penalty
FishingNorth  female.2001 NorthselFemale  0.99  Summer  event_mortality_penalty
FishingNorth  female.2002 NorthselFemale  0.99  Summer  event_mortality_penalty
FishingNorth  female.2003 NorthselFemale  0.99  Summer  event_mortality_penalty
FishingNorth  female.2004 NorthselFemale  0.99  Summer  event_mortality_penalty
FishingNorth  female.2005 NorthselFemale  0.99  Summer  event_mortality_penalty
FishingNorth  female.2006 NorthselFemale  0.99  Summer  event_mortality_penalty
FishingNorth  female.2007 NorthselFemale  0.99  Summer  event_mortality_penalty
FishingNorth  female.2008 NorthselFemale  0.99  Summer  event_mortality_penalty
FishingNorth  female.2009 NorthselFemale  0.99  Summer  event_mortality_penalty
FishingNorth  female.2010 NorthselFemale  0.99  Summer  event_mortality_penalty
FishingNorth  female.2011 NorthselFemale  0.99  Summer  event_mortality_penalty
FishingNorth  female.2012 NorthselFemale  0.99  Summer  event_mortality_penalty
FishingNorth  female.2013 NorthselFemale  0.99  Summer  event_mortality_penalty
FishingNorth  female.2014 NorthselFemale  0.99  Summer  event_mortality_penalty
end_table

@selectivity One
type constant
c 1

@selectivity MaturityMale
type logistic
a50 11.99
ato95 5.25

@selectivity MaturityFemale
type logistic
a50 16.92
ato95 7.68

@selectivity SurveyselMale
type double_normal
mu        8
sigma_l   4
sigma_r  10
alpha   1.0

@selectivity SurveyselFemale
type double_normal
mu        8
sigma_l   4
sigma_r  10
alpha   1.0

@selectivity ShelfselMale
type double_normal
mu        8
sigma_l   4
sigma_r  10
alpha   1.0

@selectivity ShelfselFemale
type double_normal
mu        8
sigma_l   4
sigma_r  10
alpha   1.0

@selectivity SlopeselMale
type double_normal
mu        8
sigma_l   4
sigma_r  10
alpha   1.0

@selectivity SlopeselFemale
type double_normal
mu        8
sigma_l   4
sigma_r  10
alpha   1.0

@selectivity NorthselMale
type double_normal
mu        8
sigma_l   4
sigma_r  10
alpha   1.0

@selectivity NorthselFemale
type double_normal
mu        8
sigma_l   4
sigma_r  10
alpha   1.0

@age_length age_length_male_untagged
type von_bertalanffy
length_weight length_weight_male
k     0.093
t0   -0.256
linf 169.07
cv_first    0.102
by_length T
distribution normal

@age_length age_length_female_untagged
type von_bertalanffy
length_weight length_weight_female
k     0.090
t0    0.021
linf 180.20
by_length T
cv_first    0.102
distribution normal

@age_length age_length_male_tagged
type von_bertalanffy
length_weight length_weight_male
k     0.093
t0   0.244
linf 169.07
by_length T
distribution normal
cv_first    0.102

@age_length age_length_female_tagged
type von_bertalanffy
length_weight length_weight_female
k     0.090
t0    0.521
linf 180.20
by_length T
cv_first    0.102
distribution normal

@length_weight length_weight_male
type basic
units tonnes
a 1.387e-008
b 2.965

@length_weight length_weight_female
type basic
units tonnes
a 7.154e-009
b 3.108

@process Tag_loss_0102
type tag_loss
categories tagyear=2001:2002
tag_loss_rate 0.039
time_step_proportions 1
selectivities One
tag_loss_type single
year 2001

@process Tag_loss_03
type tag_loss
categories tagyear=2003
tag_loss_rate 0.029
time_step_proportions 1
selectivities One
tag_loss_type single
year 2003

@process Tag_loss_recent
type tag_loss
categories tagyear=2004:2014
tag_loss_rate 0.0084
time_step_proportions 1
selectivities One
tag_loss_type single
year 2004

@process 2001Tags_shelf
type tag_by_length
years 2001
from male.untagged+female.untagged
to male.2001 female.2001
selectivities ShelfselMale ShelfselFemale
penalty tagging_penalty
initial_mortality 0.135
table proportions
year  30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2001 0.0000 0.0000 0.0058 0.1395 0.4244 0.4245 0.0058 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 175
U_max 0.999

@process 2002Tags_shelf
type tag_by_length
years 2002
from male.untagged+female.untagged
to male.2002  female.2002
selectivities ShelfselMale ShelfselFemale
penalty tagging_penalty
initial_mortality 0.135
table proportions
year  30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2002  0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0
end_table
n 2
U_max 0.999

@process 2003Tags_shelf
type tag_by_length
years 2003
from male.untagged+female.untagged
to male.2003  female.2003
selectivities ShelfselMale ShelfselFemale
penalty tagging_penalty
initial_mortality 0.124
table proportions
year  30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2003 0.0 0.0 0.0 0.0 0.5 0.5 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0
end_table
n 2
U_max 0.999

@process 2004Tags_shelf
type tag_by_length
years 2004
from male.untagged+female.untagged
to male.2004  female.2004
selectivities ShelfselMale ShelfselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2004  0.0000 0.0000 0.0533 0.2933 0.3533 0.2000 0.0535 0.0333 0.0133 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 150
U_max 0.999

@process 2005Tags_shelf
type tag_by_length
years 2005
from male.untagged+female.untagged
to male.2005  female.2005
selectivities ShelfselMale ShelfselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2005  0.0000 0.0000 0.0580 0.1546 0.3380 0.1981 0.1643 0.0531 0.0242 0.0097 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 207
U_max 0.999

@process 2006Tags_shelf
type tag_by_length
years 2006
from male.untagged+female.untagged
to male.2006  female.2006
selectivities ShelfselMale ShelfselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2006 0.0294 0.0000 0.1765 0.3235 0.2059 0.1471 0.0882 0.0000 0.0000 0.0000 0.0294 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 36
U_max 0.999

@process 2007Tags_shelf
type tag_by_length
years 2007
from male.untagged+female.untagged
to male.2007  female.2007
selectivities ShelfselMale ShelfselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2007 0.0000 0.0000 0.2679 0.2857 0.2857 0.0714 0.0714 0.0179 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 56
U_max 0.999

@process 2008Tags_shelf
type tag_by_length
years 2008
from male.untagged+female.untagged
to male.2008 female.2008
selectivities ShelfselMale ShelfselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2008 0.0172 0.0000 0.0517 0.2586 0.3622 0.2931 0.0172 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 58
U_max 0.999

@process 2009Tags_shelf
type tag_by_length
years 2009
from male.untagged+female.untagged
to male.2009  female.2009
selectivities ShelfselMale ShelfselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2009 0.0000 0.0000 0.0000 0.0616 0.1712 0.3016 0.1986 0.1712 0.0616 0.0137 0.0205 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 146
U_max 0.999

@process 2010Tags_shelf
type tag_by_length
years 2010
from male.untagged+female.untagged
to male.2010  female.2010
selectivities ShelfselMale ShelfselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2010 0.0000 0.0000 0.0125 0.1062 0.4688 0.2188 0.1000 0.0312 0.0125 0.0250 0.0188 0.0000 0.0000 0.0000 0.0062 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 161
U_max 0.999

@process 2011Tags_shelf
type tag_by_length
years 2011
from male.untagged+female.untagged
to male.2011  female.2011
selectivities ShelfselMale ShelfselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2011 0.0000 0.0000 0.0000 0.0100 0.1003 0.1774 0.1673 0.1672 0.1371 0.1070 0.0769 0.0401 0.0167 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 299
U_max 0.999

@process 2012Tags_shelf
type tag_by_length
years 2012
from male.untagged+female.untagged
to male.2012  female.2012
selectivities ShelfselMale ShelfselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2012 0.0000 0.0000 0.0037 0.0184 0.1624 0.3395 0.2030 0.0996 0.0627 0.0480 0.0369 0.0184 0.0037 0.0037 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 272
U_max 0.999

@process 2013Tags_shelf
type tag_by_length
years 2013
from male.untagged+female.untagged
to male.2013  female.2013
selectivities ShelfselMale ShelfselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2013 0.0000 0.0000 0.0035 0.0174 0.1742 0.1951 0.2752 0.1429 0.0941 0.0418 0.0314 0.0139 0.0035 0.0035 0.0000 0.0035 0.0000 0.0000 0.0000 0.0000
end_table
n 287
U_max 0.999

@process 2014Tags_shelf
type tag_by_length
years 2014
from male.untagged+female.untagged
to male.2014  female.2014
selectivities ShelfselMale ShelfselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2014 0.0000 0.0000 0.0000 0.0164 0.0779 0.2090 0.1803 0.1762 0.1434 0.0943 0.0656 0.0205 0.0164 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 244
U_max 0.999

@process 2001Tags_slope
type tag_by_length
years 2001
from male.untagged+female.untagged
to male.2001  female.2001
selectivities SlopeselMale SlopeselFemale
penalty tagging_penalty
initial_mortality 0.135
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2001  0.0000 0.0000 0.0513 0.1538 0.1026 0.1539 0.3590 0.1538 0.0256 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 39
U_max 0.999

@process 2002Tags_slope
type tag_by_length
years 2002
from male.untagged+female.untagged
to male.2002  female.2002
selectivities SlopeselMale SlopeselFemale
penalty tagging_penalty
initial_mortality 0.135
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2002 0.0000 0.0090 0.0685 0.1855 0.0595 0.1405 0.2919 0.1604 0.0631 0.0126 0.0072 0.0018 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 558
U_max 0.999

@process 2003Tags_slope
type tag_by_length
years 2003
from male.untagged+female.untagged
to male.2003  female.2003
selectivities SlopeselMale SlopeselFemale
penalty tagging_penalty
initial_mortality 0.124
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2003 0.0000 0.0065 0.0452 0.0581 0.0452 0.1065 0.3000 0.2837 0.1418 0.0065 0.0065 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 311
U_max 0.999

@process 2004Tags_slope
type tag_by_length
years 2004
from male.untagged+female.untagged
to male.2004  female.2004
selectivities SlopeselMale SlopeselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2004 0.0010 0.0039 0.1120 0.2923 0.1091 0.0936 0.1458 0.1728 0.0531 0.0106 0.0048 0.0010 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 1037
U_max 0.999

@process 2005Tags_slope
type tag_by_length
years 2005
from male.untagged+female.untagged
to male.2005  female.2005
selectivities SlopeselMale SlopeselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2005 0.0000 0.0028 0.1095 0.2420 0.1320 0.1304 0.1800 0.1033 0.0593 0.0231 0.0090 0.0068 0.0006 0.0006 0.0006 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 1780
U_max 0.999

@process 2006Tags_slope
type tag_by_length
years 2006
from male.untagged+female.untagged
to male.2006  female.2006
selectivities SlopeselMale SlopeselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2006 0.0000 0.0035 0.0525 0.1209 0.0761 0.0832 0.1091 0.1687 0.1499 0.0938 0.0832 0.0413 0.0089 0.0077 0.0000 0.0012 0.0000 0.0000 0.0000 0.0000
end_table
n 1698
U_max 0.999

@process 2007Tags_slope
type tag_by_length
years 2007
from male.untagged+female.untagged
to male.2007  female.2007
selectivities SlopeselMale SlopeselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2007 0.0005 0.0114 0.1176 0.1887 0.1555 0.1576 0.1413 0.0850 0.0435 0.0346 0.0371 0.0183 0.0069 0.0020 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 2033
U_max 0.999

@process 2008Tags_slope
type tag_by_length
years 2008
from male.untagged+female.untagged
to male.2008  female.2008
selectivities SlopeselMale SlopeselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2008 0.0000 0.0006 0.0592 0.1389 0.1757 0.1892 0.1659 0.0881 0.0559 0.0514 0.0334 0.0289 0.0122 0.0006 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 1559
U_max 0.999

@process 2009Tags_slope
type tag_by_length
years 2009
from male.untagged+female.untagged
to male.2009  female.2009
selectivities SlopeselMale SlopeselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2009 0.0000 0.0000 0.0347 0.2622 0.1575 0.1677 0.1170 0.0720 0.0373 0.0540 0.0424 0.0321 0.0186 0.0026 0.0013 0.0006 0.0000 0.0000 0.0000 0.0000
end_table
n 1567
U_max 0.999

@process 2010Tags_slope
type tag_by_length
years 2010
from male.untagged+female.untagged
to male.2010  female.2010
selectivities SlopeselMale SlopeselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2010 0.0007 0.0015 0.0066 0.0325 0.1095 0.1403 0.1625 0.1056 0.1152 0.1004 0.0916 0.0871 0.0340 0.0103 0.0022 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 1354
U_max 0.999

@process 2011Tags_slope
type tag_by_length
years 2011
from male.untagged+female.untagged
to male.2011  female.2011
selectivities SlopeselMale SlopeselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2011 0.0000 0.0007 0.0055 0.0089 0.0245 0.1280 0.1922 0.1772 0.1302 0.1132 0.0873 0.0777 0.0382 0.0123 0.0034 0.0007 0.0000 0.0000 0.0000 0.0000
end_table
n 1468
U_max 0.999

@process 2012Tags_slope
type tag_by_length
years 2012
from male.untagged+female.untagged
to male.2012  female.2012
selectivities SlopeselMale SlopeselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2012 0.0000 0.0000 0.0000 0.0119 0.0183 0.0970 0.2415 0.2242 0.1471 0.1176 0.0787 0.0390 0.0167 0.0064 0.0016 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 1258
U_max 0.999

@process 2013Tags_slope
type tag_by_length
years 2013
from male.untagged+female.untagged
to male.2013  female.2013
selectivities SlopeselMale SlopeselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2013 0.0000 0.0014 0.0027 0.0130 0.0363 0.0685 0.1541 0.1986 0.1849 0.1466 0.1055 0.0507 0.0288 0.0075 0.0007 0.0007 0.0000 0.0000 0.0000 0.0000
end_table
n 1460
U_max 0.999

@process 2014Tags_slope
type tag_by_length
years 2014
from male.untagged+female.untagged
to male.2014  female.2014
selectivities SlopeselMale SlopeselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2014 0.0000 0.0000 0.0021 0.0197 0.0254 0.0557 0.1317 0.1910 0.1966 0.1614 0.1163 0.0677 0.0197 0.0085 0.0028 0.0007 0.0007 0.0000 0.0000 0.0000
end_table
n 1420
U_max 0.999

@process 2001Tags_north
type tag_by_length
years 2001
from male.untagged+female.untagged
to male.2001 female.2001
selectivities NorthselMale NorthselFemale
penalty tagging_penalty
initial_mortality 0.135
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2001 0.0000 0.0000 0.0000 0.0000 0.2857 0.5714 0.1429 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 7
U_max 0.999

@process 2002Tags_north
type tag_by_length
years 2002
from male.untagged+female.untagged
to male.2002  female.2002
selectivities NorthselMale NorthselFemale
penalty tagging_penalty
initial_mortality 0.135
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2002 0.0000 0.0196 0.0196 0.0392 0.1176 0.1176 0.3139 0.2157 0.0588 0.0784 0.0196 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 51
U_max 0.999

@process 2003Tags_north
type tag_by_length
years 2003
from male.untagged+female.untagged
to male.2003 female.2003
selectivities NorthselMale NorthselFemale
penalty tagging_penalty
initial_mortality 0.124
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2003 0.0022 0.0000 0.0065 0.0065 0.0239 0.0369 0.0629 0.1129 0.1561 0.3037 0.1800 0.0672 0.0347 0.0065 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 470
U_max 0.999

@process 2004Tags_north
type tag_by_length
years 2004
from male.untagged+female.untagged
to male.2004 female.2004
selectivities NorthselMale NorthselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2004 0.0000 0.0000 0.0184 0.0276 0.0461 0.0691 0.0783 0.1336 0.0968 0.2353 0.1613 0.0875 0.0276 0.0000 0.0138 0.0046 0.0000 0.0000 0.0000 0.0000
end_table
n 221
U_max 0.999

@process 2005Tags_north
type tag_by_length
years 2005
from male.untagged+female.untagged
to male.2005 female.2005
selectivities NorthselMale NorthselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2005 0.0000 0.0000 0.0074 0.0074 0.0112 0.0186 0.0483 0.0818 0.1338 0.2305 0.2194 0.1673 0.0483 0.0223 0.0037 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 269
U_max 0.999

@process 2006Tags_north
type tag_by_length
years 2006
from male.untagged+female.untagged
to male.2006 female.2006
selectivities NorthselMale NorthselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2006 0.0000 0.0000 0.0000 0.0026 0.0000 0.0026 0.0052 0.0234 0.0805 0.1974 0.3064 0.2156 0.1143 0.0364 0.0104 0.0052 0.0000 0.0000 0.0000 0.0000
end_table
n 385
U_max 0.999

@process 2007Tags_north
type tag_by_length
years 2007
from male.untagged+female.untagged
to male.2007 female.2007
selectivities NorthselMale NorthselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2007 0.0000 0.0035 0.0158 0.0351 0.0281 0.0123 0.0193 0.0281 0.0492 0.2425 0.3148 0.1722 0.0562 0.0211 0.0018 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 570
U_max 0.999

@process 2008Tags_north
type tag_by_length
years 2008
from male.untagged+female.untagged
to male.2008 female.2008
selectivities NorthselMale NorthselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2008 0.0000 0.0047 0.0047 0.0047 0.0093 0.0093 0.0233 0.0186 0.0698 0.2184 0.2744 0.2140 0.1023 0.0465 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 215
U_max 0.999

@process 2009Tags_north
type tag_by_length
years 2009
from male.untagged+female.untagged
to male.2009 female.2009
selectivities NorthselMale NorthselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2009 0.0000 0.0000 0.0179 0.0090 0.0179 0.0179 0.0045 0.0135 0.0716 0.2691 0.3274 0.1480 0.0942 0.0090 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 223
U_max 0.999

@process 2010Tags_north
type tag_by_length
years 2010
from male.untagged+female.untagged
to male.2010 female.2010
selectivities NorthselMale NorthselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2010 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0131 0.0079 0.0656 0.2205 0.3411 0.2415 0.0866 0.0079 0.0079 0.0079 0.0000 0.0000 0.0000 0.0000
end_table
n 381
U_max 0.999

@process 2011Tags_north
type tag_by_length
years 2011
from male.untagged+female.untagged
to male.2011 female.2011
selectivities NorthselMale NorthselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2011 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0186 0.0186 0.0435 0.1304 0.2982 0.2795 0.1491 0.0621 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000
end_table
n 161
U_max 0.999

@process 2012Tags_north
type tag_by_length
years 2012
from male.untagged+female.untagged
to male.2012 female.2012
selectivities NorthselMale NorthselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2012 0.0000 0.0000 0.0000 0.0064 0.0000 0.0043 0.0064 0.0043 0.0451 0.1352 0.2382 0.3069 0.1867 0.0494 0.0150 0.0021 0.0000 0.0000 0.0000 0.0000
end_table
n 469
U_max 0.999

@process 2013Tags_north
type tag_by_length
years 2013
from male.untagged+female.untagged
to male.2013 female.2013
selectivities NorthselMale NorthselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2013 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0000 0.0110 0.0166 0.0967 0.2569 0.3039 0.1906 0.0718 0.0359 0.0166 0.0000 0.0000 0.0000 0.0000
end_table
n 362
U_max 0.999

@process 2014Tags_north
type tag_by_length
years 2014
from male.untagged+female.untagged
to male.2014 female.2014
selectivities NorthselMale NorthselFemale
penalty tagging_penalty
initial_mortality 0.1
table proportions
year 30 40 50 60 70 80 90 100 110 120 130 140 150 160 170 180 190 200 210 220
2014 0.0000 0.0000 0.0000 0.0000 0.0029 0.0000 0.0000 0.0000 0.0000 0.1295 0.2588 0.3176 0.2118 0.0559 0.0176 0.0059 0.0000 0.0000 0.0000 0.0000
end_table
n 340
U_max 0.999

@penalty event_mortality_penalty
type process
log_scale True
multiplier 10

@assert male.untagged
type partition
values 0.000000 684408.887831 596633.411516 516764.466095 443994.732346 378255.478027 319700.913403 268899.698663 226017.698654 190226.248828 160523.568415 135824.409861 115406.293664 98487.674349 84471.449257 72929.529191 63452.113959 55702.072763 49320.372072 43996.321708 39481.951054 35597.842454 32201.869533 29189.029783 26480.317273 24016.136122 21758.506542 19682.052635 17768.602199 16004.928105 14381.213499 12889.792092 11524.174469 10278.349451 9146.331461 8121.912979 7198.575492 6369.512319 5627.721011 4966.130266 4377.734801 3855.720107 3393.566413 2985.127174 2624.681611 2306.963592 2027.170618 1780.957181 1564.416713 11275.788016

@assert male.2001
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.007857 0.304875 1.582883 2.952117 2.875658 1.792926 0.899754 0.428276 0.203552 0.097832 0.047637 0.023517 0.011782 0.005995 0.003100 0.001627 0.000867 0.000468 0.000256 0.000141 0.000079 0.000044 0.000025 0.000014 0.000008 0.000005 0.000003 0.000001 0.000001 0.000000 0.000000 0.000000 0.000000

@assert male.2002
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000901 0.361310 2.355876 4.229494 3.795742 4.848950 5.854460 5.265461 3.928140 2.641750 1.687446 1.054813 0.655219 0.406996 0.253171 0.157621 0.098113 0.061000 0.037852 0.023430 0.014461 0.008896 0.005452 0.003328 0.002023 0.001223 0.000736 0.000440 0.000262 0.000154 0.000091 0.000053 0.000030 0.000017 0.000022

@assert male.2003
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.051071 0.213456 1.134267 1.636782 2.364185 4.194778 6.262027 7.344598 7.631515 7.462998 6.936971 6.105670 5.102451 4.080918 3.150310 2.363295 1.731232 1.242546 0.875797 0.607256 0.414751 0.279321 0.185645 0.121847 0.079018 0.050652 0.032103 0.020122 0.012475 0.007650 0.004640 0.002784 0.001652 0.000969 0.000562 0.000732

@assert male.2004
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.081765 1.262847 16.331922 30.944682 24.784588 19.251167 17.857224 16.409096 13.705854 10.644018 8.057499 6.089672 4.606037 3.467560 2.586200 1.908324 1.394793 1.011943 0.730029 0.524025 0.374108 0.265300 0.186583 0.129928 0.089457 0.060832 0.040823 0.027021 0.017636 0.011348 0.007198 0.004501 0.002774 0.001686 0.001010 0.000597 0.000800

@assert male.2005
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.001966 2.192162 29.229201 51.691749 46.279606 40.672222 36.786173 30.353308 23.720794 18.366820 14.424840 11.508853 9.236396 7.368564 5.795611 4.476514 3.391947 2.522259 1.842222 1.322851 0.934670 0.650266 0.445726 0.301167 0.200677 0.131916 0.085575 0.054797 0.034642 0.021626 0.013333 0.008119 0.004883 0.002901 0.001702 0.000986 0.000564 0.000711

@assert male.2006
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.118729 1.706006 16.135048 28.335695 25.609662 24.587081 27.505702 30.830224 32.403115 32.060696 30.562813 28.258007 25.260559 21.779961 18.131522 14.623038 11.470493 8.782410 6.582267 4.839554 3.496101 2.484249 1.737726 1.197261 0.812832 0.543950 0.358903 0.233534 0.149886 0.094901 0.059285 0.036544 0.022229 0.013344 0.007906 0.004623 0.002668 0.001519 0.001890

@assert male.2007
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.147036 6.745949 48.069961 72.093390 68.061336 59.553547 48.913190 37.799666 30.314610 27.071450 26.223355 25.552217 23.905308 21.206339 17.891923 14.459168 11.272082 8.526450 6.285795 4.531198 3.201810 2.221843 1.516291 1.018783 0.674507 0.440345 0.283621 0.180305 0.113174 0.070156 0.042958 0.025987 0.015531 0.009171 0.005351 0.003085 0.001757 0.000988 0.000549 0.000653

@assert male.2008
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.156942 1.728283 23.196967 49.234201 61.451047 60.794318 50.132126 37.206154 28.046347 22.852642 20.021795 18.087843 16.256993 14.256530 12.105693 9.938758 7.899371 6.093543 4.574941 3.351818 2.401883 1.686723 1.162674 0.787724 0.525129 0.344764 0.223079 0.142341 0.089608 0.055676 0.034153 0.020688 0.012377 0.007314 0.004269 0.002462 0.001402 0.000789 0.000439 0.000241 0.000281

@assert male.2009
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.950607 28.203691 82.996037 82.349626 70.184369 54.155597 39.387402 30.142534 25.699211 23.738258 22.266585 20.372108 17.942234 15.197488 12.418801 9.828882 7.561792 5.673917 4.163834 2.995143 2.115411 1.468881 1.003732 0.675479 0.447938 0.292840 0.188802 0.120079 0.075356 0.046671 0.028530 0.017217 0.010257 0.006033 0.003504 0.002009 0.001137 0.000636 0.000351 0.000191 0.000222

@assert male.2010
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.215424 0.838393 5.788481 27.130411 54.940528 63.860016 58.988492 50.483590 45.740933 44.722953 45.086615 44.708932 42.535558 38.541614 33.330831 27.661076 22.163945 17.239324 13.071370 9.693875 7.048942 5.034457 3.535869 2.444010 1.663467 1.115330 0.736893 0.479872 0.308079 0.195029 0.121762 0.074985 0.045555 0.027306 0.016151 0.009427 0.005430 0.003087 0.001732 0.000959 0.000524 0.000283 0.000328

@assert male.2011
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.001195 0.497866 3.272070 10.453426 35.516142 70.596917 88.616652 87.331740 78.487551 68.826842 60.619523 53.729405 47.398077 41.139032 34.878984 28.809962 23.180354 18.188370 13.938733 10.447813 7.670525 5.523059 3.904322 2.712012 1.852336 1.244749 0.823366 0.536341 0.344184 0.217666 0.135697 0.083416 0.050573 0.030247 0.017848 0.010392 0.005972 0.003387 0.001896 0.001047 0.000571 0.000307 0.000163 0.000192

@assert male.2012
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.029339 2.048980 13.574852 41.812932 83.406829 107.507215 105.854108 94.029201 82.191424 73.157816 66.021682 59.404002 52.506235 45.223208 37.852583 30.809235 24.425394 18.898810 14.296400 10.588045 7.686948 5.476598 3.832302 2.635711 1.782663 1.186265 0.776993 0.501117 0.318344 0.199263 0.122929 0.074764 0.044837 0.026521 0.015474 0.008907 0.005059 0.002835 0.001568 0.000856 0.000461 0.000245 0.000129 0.000161

@assert male.2013
type partition
values 0.000000 0.000000 0.000000 0.000000 0.003390 1.055761 4.452370 21.001992 52.340037 87.251225 115.583754 125.182740 121.764752 112.438090 101.556112 90.479939 79.333194 68.251371 57.497225 47.416201 38.306424 30.358871 23.628280 18.073566 13.591971 10.049736 7.306075 5.222685 3.671088 2.537473 1.724822 1.153118 0.758336 0.490680 0.312453 0.195855 0.120882 0.073482 0.044004 0.025967 0.015102 0.008658 0.004894 0.002728 0.001499 0.000813 0.000435 0.000229 0.000119 0.000170

@assert male.2014
type partition
values 0.000000 0.000000 0.000000 0.000000 0.121735 4.792081 21.325551 45.069269 80.922354 114.642316 133.667651 139.131572 135.302508 126.200341 114.020940 100.035080 85.274208 70.781814 57.336157 45.430540 35.285203 26.917114 20.193995 14.912500 10.844285 7.766034 5.477449 3.805015 2.603303 1.754139 1.164017 0.760695 0.489591 0.310357 0.193796 0.119219 0.072265 0.043169 0.025418 0.014754 0.008444 0.004765 0.002652 0.001456 0.000788 0.000421 0.000222 0.000115 0.000059 0.000105

@assert female.untagged
type partition
values 0.000000 684408.887831 596633.411516 516764.466095 443994.789598 378256.100089 319701.489208 268899.798287 226014.442814 190218.502666 160513.066539 135811.296304 115394.895652 98477.853063 84462.816557 72919.546349 63440.287756 55689.219536 49307.160573 43984.370444 39472.307437 35591.529182 32198.932551 29188.725993 26481.863106 24018.830377 21761.735448 19685.334797 17771.617916 16007.509140 14383.307307 12891.420283 11525.397542 10279.241866 9146.966490 8122.354985 7198.877101 6369.714425 5627.854178 4966.216628 4377.789971 3855.754843 3393.587979 2985.140381 2624.689591 2306.968351 2027.173419 1780.958808 1564.417646 11275.789220

@assert female.2001
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert female.2002
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert female.2003
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert female.2004
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert female.2005
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert female.2006
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert female.2007
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert female.2008
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert female.2009
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert female.2010
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert female.2011
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert female.2012
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert female.2013
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert female.2014
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000
)";

} /* namespace testcases */
} /* namespace niwa */

#endif /* TESTMODE */
#endif /* SOURCE_TESTRESOURCES_MODELS_ROSSSEACOMPLEX_H_ */
