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
standardise_years 2004:2010
recruitment_multipliers 1*21
steepness 0.75
ssb SSB
age 1

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
FishingShelf  format=male.* ShelfselMale  0.99  Summer  event_mortality_penalty
FishingShelf  format=female.* ShelfselFemale  0.99  Summer  event_mortality_penalty
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
values 0.000000 684383.067084 596614.132749 516751.225593 443986.799874 378252.064797 319701.262438 268902.316869 226021.750444 190231.314282 160529.163008 135829.767981 115411.137662 98491.999637 84475.137758 72932.610429 63454.659834 55704.160955 49322.074465 43997.701060 39483.061060 35598.728863 32202.571079 29189.579416 26480.743062 24016.461926 21758.752605 19682.235970 17768.736907 16005.025688 14381.283179 12889.841134 11524.208491 10278.372717 9146.347147 8121.923406 7198.582328 6369.516740 5627.723832 4966.132041 4377.735904 3855.720783 3393.566823 2985.127419 2624.681756 2306.963677 2027.170667 1780.957209 1564.416728 11275.788036

@assert male.2001
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.005639 0.168329 0.801553 1.469641 1.462672 0.949761 0.496133 0.244301 0.119686 0.059220 0.029657 0.015034 0.007719 0.004015 0.002117 0.001131 0.000612 0.000335 0.000185 0.000103 0.000058 0.000033 0.000019 0.000011 0.000006 0.000003 0.000002 0.000001 0.000001 0.000000 0.000000 0.000000 0.000000

@assert male.2002
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000876 0.211408 1.230844 2.111082 1.899571 2.370885 2.886645 2.677552 2.066954 1.434110 0.940626 0.600970 0.380231 0.240076 0.151655 0.095845 0.060544 0.038186 0.024026 0.015069 0.009417 0.005860 0.003631 0.002238 0.001373 0.000837 0.000508 0.000306 0.000183 0.000109 0.000064 0.000037 0.000022 0.000012 0.000016

@assert male.2003
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.032432 0.121160 0.580436 0.819755 1.169746 2.021303 3.005104 3.550830 3.703402 3.629057 3.398629 3.033026 2.578434 2.098904 1.647776 1.255759 0.933664 0.679667 0.485624 0.341167 0.235972 0.160848 0.108137 0.071748 0.047005 0.030420 0.019452 0.012293 0.007680 0.004743 0.002895 0.001748 0.001043 0.000615 0.000358 0.000471

@assert male.2004
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.052150 0.819516 8.582845 15.456864 12.499898 9.693788 8.876433 8.215710 7.001787 5.528937 4.219995 3.199778 2.427599 1.836899 1.379167 1.024121 0.751476 0.545310 0.391875 0.279241 0.197449 0.138550 0.096436 0.066529 0.045447 0.030713 0.020517 0.013538 0.008820 0.005671 0.003597 0.002251 0.001389 0.000845 0.000507 0.000300 0.000404

@assert male.2005
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.001911 1.467835 15.265347 25.837738 23.256824 20.389072 18.466000 15.435366 12.194952 9.474296 7.421019 5.893785 4.718370 3.770155 2.981312 2.320954 1.774710 1.332233 0.982219 0.711763 0.507341 0.355966 0.245993 0.167518 0.112464 0.074461 0.048635 0.031345 0.019938 0.012519 0.007760 0.004749 0.002870 0.001712 0.001009 0.000587 0.000337 0.000427

@assert male.2006
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.076013 1.056274 8.421460 14.162647 12.854333 12.207090 13.366884 14.855543 15.637836 15.487327 14.746610 13.647513 12.268030 10.678978 8.994216 7.342347 5.826572 4.508321 3.410353 2.527615 1.838593 1.314271 0.924130 0.639662 0.436094 0.292962 0.193996 0.126660 0.081555 0.051796 0.032452 0.020060 0.012235 0.007364 0.004373 0.002563 0.001482 0.000846 0.001057

@assert male.2007
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.097765 4.081871 24.794580 36.033260 34.141647 29.962433 24.767944 19.266190 15.255857 13.171666 12.407972 12.010833 11.348492 10.249765 8.828251 7.283929 5.791766 4.462708 3.346881 2.451323 1.757873 1.236640 0.854705 0.581049 0.388896 0.256449 0.166713 0.106894 0.067626 0.042226 0.026029 0.015843 0.009522 0.005652 0.003314 0.001919 0.001097 0.000620 0.000346 0.000413

@assert male.2008
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.099665 1.141738 12.164147 24.724812 30.678155 30.477081 25.437764 19.090215 14.329206 11.440136 9.799056 8.729213 7.817828 6.889197 5.912511 4.924078 3.978236 3.122368 2.385593 1.778067 1.295375 0.924053 0.646410 0.444007 0.299789 0.199152 0.130266 0.083953 0.053337 0.033419 0.020658 0.012602 0.007588 0.004511 0.002647 0.001534 0.000878 0.000496 0.000277 0.000152 0.000179

@assert male.2009
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.681619 15.349913 41.610244 41.310551 35.389171 27.581961 20.188388 15.261093 12.629577 11.376878 10.578527 9.718460 8.656015 7.440943 6.180832 4.974796 3.890965 2.965610 2.208159 1.609571 1.150497 0.807502 0.557124 0.378168 0.252721 0.166366 0.107933 0.069035 0.043545 0.027095 0.016634 0.010077 0.006025 0.003555 0.002071 0.001191 0.000676 0.000379 0.000209 0.000114 0.000133

@assert male.2010
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.136861 0.461306 3.126176 13.770703 27.264017 31.756259 29.488775 25.141859 22.283584 21.193212 20.978517 20.708801 19.838067 18.233018 16.053883 13.579990 11.085234 8.771061 6.752650 5.074881 3.732796 2.692438 1.907133 1.327983 0.909732 0.613470 0.407406 0.266545 0.171851 0.109216 0.068433 0.042285 0.025769 0.015491 0.009187 0.005376 0.003104 0.001768 0.000994 0.000551 0.000302 0.000163 0.000190

@assert male.2011
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.001162 0.295791 1.705096 5.320525 17.366822 34.162144 43.188422 42.981594 38.795029 33.879598 29.555850 25.954172 22.788388 19.803791 16.898606 14.099502 11.484167 9.131669 7.093833 5.388845 4.007674 2.921180 2.088892 1.466637 1.011772 0.686213 0.457806 0.300576 0.194296 0.123701 0.077595 0.047971 0.029236 0.017570 0.010413 0.006088 0.003511 0.001998 0.001122 0.000622 0.000340 0.000183 0.000098 0.000116

@assert male.2012
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.021159 1.155727 6.907334 20.470399 40.259785 52.292595 52.151031 46.522998 40.326936 35.347258 31.462797 28.112466 24.865595 21.563061 18.245100 15.044971 12.095454 9.491825 7.280069 5.463414 4.016355 2.895253 2.048316 1.423203 0.971747 0.652347 0.430766 0.279913 0.179057 0.112797 0.069998 0.042804 0.025799 0.015330 0.008982 0.005191 0.002959 0.001664 0.000923 0.000505 0.000273 0.000145 0.000076 0.000096

@assert male.2013
type partition
values 0.000000 0.000000 0.000000 0.000000 0.003295 0.581828 2.379210 10.681443 25.795394 42.255791 55.687718 60.579126 59.132683 54.507963 48.959788 43.390374 37.965200 32.719726 27.699384 22.998175 18.719852 14.946652 11.713585 9.015936 6.818419 5.067204 3.701263 2.657691 1.876206 1.302313 0.888893 0.596671 0.393949 0.255888 0.163552 0.102889 0.063723 0.038863 0.023346 0.013817 0.008058 0.004632 0.002625 0.001466 0.000808 0.000439 0.000235 0.000124 0.000065 0.000093

@assert male.2014
type partition
values 0.000000 0.000000 0.000000 0.000000 0.087402 2.647484 10.784430 22.190159 39.002409 54.765399 63.833791 66.537651 64.659552 60.176923 54.316397 47.767381 40.972465 34.326370 28.117725 22.545350 17.717264 13.665373 10.355539 7.715844 5.655197 4.077813 2.893385 2.020492 1.388747 0.939572 0.625752 0.410271 0.264836 0.168335 0.105373 0.064970 0.039465 0.023621 0.013933 0.008101 0.004643 0.002624 0.001463 0.000804 0.000436 0.000233 0.000123 0.000064 0.000033 0.000058

@assert female.untagged
type partition
values 0.000000 684383.067084 596614.132749 516751.225593 443986.857125 378252.686856 319701.838242 268902.416508 226018.494641 190223.568180 160518.661203 135816.654422 115399.739661 98482.178346 84466.505041 72922.627464 63442.833396 55691.307422 49308.862624 43985.749474 39473.417174 35592.415400 32199.633975 29189.275552 26482.288851 24019.156154 21761.981493 19685.518118 17771.752612 16007.606712 14383.376978 12891.469317 11525.431558 10279.265127 9146.982172 8122.365410 7198.883935 6369.718845 5627.856998 4966.218404 4377.791075 3855.755520 3393.588389 2985.140626 2624.689736 2306.968435 2027.173467 1780.958836 1564.417662 11275.789240

@assert female.2001
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.002218 0.136557 0.781380 1.482559 1.413058 0.843204 0.403638 0.183982 0.083869 0.038614 0.017981 0.008483 0.004064 0.001980 0.000982 0.000496 0.000255 0.000133 0.000070 0.000038 0.000020 0.000011 0.000006 0.000003 0.000002 0.000001 0.000001 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert female.2002
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000025 0.149916 1.125120 2.118557 1.896290 2.478201 2.967961 2.588024 1.861260 1.207682 0.746842 0.453856 0.274994 0.166922 0.101517 0.061777 0.037569 0.022814 0.013826 0.008361 0.005044 0.003035 0.001822 0.001090 0.000650 0.000386 0.000228 0.000134 0.000079 0.000046 0.000027 0.000015 0.000009 0.000005 0.000006

@assert female.2003
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.018641 0.092305 0.553876 0.817088 1.194521 2.173607 3.257099 3.793950 3.928279 3.834080 3.538453 3.072726 2.524074 1.982051 1.502557 1.107550 0.797576 0.562883 0.390176 0.266091 0.178780 0.118474 0.077508 0.050099 0.032013 0.020233 0.012652 0.007829 0.004795 0.002907 0.001744 0.001036 0.000609 0.000354 0.000204 0.000261

@assert female.2004
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.029619 0.443386 7.749762 15.489050 12.285611 9.558036 8.981344 8.193841 6.704403 5.115309 3.837654 2.889991 2.178499 1.630699 1.207056 0.884216 0.643325 0.466637 0.338157 0.244785 0.176661 0.126750 0.090147 0.063399 0.044010 0.030118 0.020306 0.013483 0.008816 0.005677 0.003601 0.002250 0.001385 0.000841 0.000503 0.000297 0.000396

@assert female.2005
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000055 0.724422 13.965114 25.856159 23.024601 20.284636 18.321406 14.918863 11.526485 8.892963 7.004123 5.615274 4.518167 3.598504 2.814361 2.155599 1.617260 1.190039 0.860011 0.611092 0.427331 0.294301 0.199734 0.133649 0.088214 0.057455 0.036940 0.023451 0.014704 0.009107 0.005573 0.003369 0.002013 0.001188 0.000693 0.000399 0.000227 0.000283

@assert female.2006
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.042721 0.649804 7.714285 14.174251 12.756372 12.380932 14.139795 15.975684 16.766230 16.574208 15.816908 14.611060 12.992962 11.101298 9.137524 7.280835 5.644011 4.274144 3.171945 2.311957 1.657517 1.169982 0.813598 0.557600 0.376738 0.250987 0.164907 0.106874 0.068330 0.043105 0.026832 0.016484 0.009994 0.005981 0.003533 0.002060 0.001185 0.000673 0.000834

@assert female.2007
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.049277 2.664350 23.277377 36.063169 33.922476 29.593430 24.147026 18.534747 15.059682 13.900531 13.816026 13.541933 12.557258 10.956906 9.063905 7.175392 5.480411 4.063798 2.938944 2.079891 1.443945 0.985206 0.661587 0.437734 0.285610 0.183896 0.116907 0.073411 0.045548 0.027930 0.016929 0.010144 0.006009 0.003519 0.002037 0.001166 0.000660 0.000369 0.000204 0.000240

@assert female.2008
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.057282 0.586608 11.033735 24.511363 30.775376 30.319610 24.696217 18.117224 13.718032 11.413164 10.223256 9.359043 8.439486 7.367575 6.193354 5.014794 3.921208 2.971218 2.189371 1.573762 1.106513 0.762671 0.516264 0.343716 0.225339 0.145611 0.092812 0.058388 0.036271 0.022257 0.013495 0.008086 0.004789 0.002803 0.001622 0.000928 0.000524 0.000293 0.000162 0.000088 0.000102

@assert female.2009
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.269016 12.854767 41.388944 41.042245 34.797893 26.575632 19.200381 14.882411 13.070391 12.362013 11.688586 10.654070 9.286538 7.756771 6.238120 4.854180 3.670881 2.708336 1.955688 1.385577 0.964915 0.661378 0.446606 0.297309 0.195215 0.126473 0.080868 0.051044 0.031811 0.019576 0.011896 0.007140 0.004232 0.002478 0.001433 0.000818 0.000462 0.000257 0.000141 0.000077 0.000089

@assert female.2010
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.078567 0.377110 2.662472 13.360585 27.678392 32.105960 29.501763 25.343408 23.458772 23.531031 24.109286 24.001190 22.698378 20.309288 17.277449 14.081423 11.078921 8.468383 6.318780 4.619018 3.316151 2.342014 1.628728 1.116019 0.753728 0.501854 0.329483 0.213325 0.136226 0.085812 0.053328 0.032700 0.019786 0.011815 0.006963 0.004051 0.002326 0.001318 0.000738 0.000407 0.000222 0.000120 0.000138

@assert female.2011
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000033 0.202086 1.567053 5.133160 18.150291 36.436819 45.430844 44.352769 39.694791 34.949108 31.065184 27.776445 24.610636 21.335951 17.980883 14.710795 11.696394 9.056815 6.844953 5.058985 3.662847 2.601866 1.815415 1.245361 0.840552 0.558527 0.365553 0.235759 0.149885 0.093963 0.058101 0.035444 0.021337 0.012677 0.007434 0.004304 0.002460 0.001388 0.000774 0.000426 0.000231 0.000124 0.000066 0.000077

@assert female.2012
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.008180 0.893282 6.667744 21.343302 43.148882 55.217170 53.705641 47.508532 41.866437 37.812171 34.560215 31.292602 27.641454 23.660729 19.607867 15.764492 12.330054 9.407024 7.016326 5.124604 3.670558 2.581310 1.783956 1.212483 0.810898 0.533905 0.346218 0.221197 0.139283 0.086463 0.052930 0.031959 0.019038 0.011190 0.006491 0.003716 0.002100 0.001172 0.000645 0.000351 0.000188 0.000100 0.000052 0.000065

@assert female.2013
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000095 0.473940 2.073198 10.320734 26.545198 44.996529 59.897863 64.605766 62.634216 57.932191 52.598114 47.091035 41.369145 35.532493 29.798422 24.418385 19.586763 15.412290 11.914691 9.057583 6.773486 4.982463 3.604748 2.564940 1.794839 1.235128 0.835905 0.556430 0.364375 0.234784 0.148896 0.092963 0.057157 0.034617 0.020657 0.012149 0.007043 0.004026 0.002269 0.001261 0.000692 0.000374 0.000200 0.000105 0.000055 0.000077

@assert female.2014
type partition
values 0.000000 0.000000 0.000000 0.000000 0.034332 2.144586 10.541167 22.879206 41.920128 59.877475 69.835028 72.595367 70.644461 66.024955 59.705879 52.268752 44.302497 36.455923 29.218684 22.885274 17.567910 13.251648 9.838335 7.196529 5.188972 3.688121 2.583983 1.784461 1.214510 0.814533 0.538242 0.350408 0.224745 0.142015 0.088419 0.054246 0.032799 0.019547 0.011484 0.006653 0.003800 0.002141 0.001189 0.000652 0.000352 0.000188 0.000099 0.000051 0.000026 0.000047
)";

} /* namespace testcases */
} /* namespace niwa */

#endif /* TESTMODE */
#endif /* SOURCE_TESTRESOURCES_MODELS_ROSSSEACOMPLEX_H_ */
