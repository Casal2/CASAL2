/**
 * @file RossSeaComplex.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 10/10/2018
 * @section LICENSE
 *
 * Copyright NIWA Science ©2018 - www.niwa.co.nz
 *
 * @section
 *
 * This is testing a Complex CASAL2 model with instant mortality
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
 *
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
categories tagyear=untagged
proportions 0.5 0.5 
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
selectivities One*30
categories *
time_step_ratio 0.5 0.5 
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
time_step_ratio 1
selectivities One
tag_loss_type single
year 2001

@process Tag_loss_03
type tag_loss
categories tagyear=2003
tag_loss_rate 0.029
time_step_ratio 1
selectivities One
tag_loss_type single
year 2003

@process Tag_loss_recent
type tag_loss
categories tagyear=2004:2014
tag_loss_rate 0.0084
time_step_ratio 1
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
values 0.000000 684358.903620 596594.446122 516734.225436 443970.948724 378236.076459 319684.097425 268882.629846 225998.018413 190202.597107 160492.508383 135783.855746 115361.413269 98443.235552 84429.355417 72890.710324 63417.605493 55671.957789 49293.696787 43972.581417 39460.722579 35579.281250 32186.238784 29176.243746 26470.085984 24008.115574 21752.346062 19677.414608 17765.176740 16002.443681 14379.442096 12888.549246 11523.315671 10277.764596 9145.938684 8121.652723 7198.405281 6369.402405 5627.650910 4966.086099 4377.707307 3855.703194 3393.556131 2985.120997 2624.677942 2306.961439 2027.169369 1780.956465 1564.416307 11275.787503

@assert male.2001 
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.005639 0.168317 0.801501 1.469554 1.462595 0.949717 0.496113 0.244293 0.119682 0.059218 0.029656 0.015034 0.007719 0.004015 0.002117 0.001131 0.000612 0.000335 0.000185 0.000103 0.000058 0.000033 0.000019 0.000011 0.000006 0.000003 0.000002 0.000001 0.000001 0.000000 0.000000 0.000000 0.000000

@assert male.2002 
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000876 0.211391 1.230752 2.110936 1.899449 2.370739 2.886483 2.677425 2.066874 1.434065 0.940603 0.600957 0.380225 0.240073 0.151654 0.095844 0.060544 0.038186 0.024026 0.015069 0.009417 0.005860 0.003631 0.002238 0.001373 0.000837 0.000508 0.000306 0.000183 0.000109 0.000064 0.000037 0.000022 0.000012 0.000016

@assert male.2003 
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.032429 0.121150 0.580391 0.819695 1.169668 2.021183 3.004920 3.550608 3.703196 3.628893 3.398511 3.032949 2.578388 2.098880 1.647764 1.255755 0.933663 0.679668 0.485625 0.341168 0.235973 0.160849 0.108138 0.071749 0.047006 0.030420 0.019452 0.012293 0.007680 0.004743 0.002895 0.001748 0.001043 0.000615 0.000358 0.000471

@assert male.2004 
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.052145 0.819447 8.582156 15.455657 12.498957 9.693129 8.875898 8.215191 7.001333 5.528619 4.219800 3.199666 2.427539 1.836870 1.379157 1.024121 0.751480 0.545316 0.391881 0.279246 0.197452 0.138553 0.096438 0.066530 0.045448 0.030714 0.020517 0.013539 0.008820 0.005671 0.003597 0.002251 0.001389 0.000845 0.000507 0.000300 0.000404

@assert male.2005 
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.001911 1.467718 15.264203 25.835709 23.254845 20.387565 18.464804 15.434409 12.194120 9.473639 7.420578 5.893518 4.718219 3.770078 2.981281 2.320952 1.774723 1.332252 0.982238 0.711780 0.507355 0.355977 0.246001 0.167524 0.112468 0.074464 0.048636 0.031346 0.019939 0.012520 0.007761 0.004750 0.002870 0.001713 0.001009 0.000587 0.000337 0.000427

@assert male.2006 
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.076007 1.056192 8.420877 14.161606 12.853213 12.206070 13.365948 14.854594 15.636868 15.486280 14.745599 13.646714 12.267488 10.678660 8.994068 7.342319 5.826618 4.508404 3.410448 2.527705 1.838671 1.314334 0.924179 0.639697 0.436119 0.292980 0.194008 0.126668 0.081560 0.051800 0.032454 0.020062 0.012236 0.007364 0.004373 0.002563 0.001482 0.000846 0.001057

@assert male.2007 
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.097758 4.081564 24.792791 36.030807 34.139081 29.959940 24.765929 19.264798 15.254802 13.170772 12.407068 12.009961 11.347799 10.249310 8.828014 7.283862 5.791814 4.462819 3.347017 2.451457 1.757990 1.236736 0.854779 0.581104 0.388935 0.256475 0.166731 0.106905 0.067633 0.042231 0.026032 0.015845 0.009523 0.005653 0.003314 0.001919 0.001097 0.000620 0.000346 0.000413

@assert male.2008
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.099658 1.141666 12.163421 24.723263 30.676149 30.474819 25.435547 19.088574 14.328178 11.439407 9.798453 8.728611 7.817267 6.888768 5.912241 4.923950 3.978215 3.122419 2.385683 1.778170 1.295473 0.924138 0.646479 0.444059 0.299827 0.199179 0.130284 0.083965 0.053344 0.033424 0.020661 0.012604 0.007589 0.004511 0.002648 0.001534 0.000878 0.000496 0.000277 0.000152 0.000179

@assert male.2009
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.681579 15.349156 41.608051 41.307819 35.386579 27.579580 20.186435 15.259759 12.628743 11.376245 10.577949 9.717841 8.655432 7.440518 6.180592 4.974713 3.890993 2.965703 2.208282 1.609697 1.150611 0.807596 0.557198 0.378223 0.252760 0.166393 0.107951 0.069047 0.043553 0.027100 0.016637 0.010079 0.006026 0.003556 0.002071 0.001191 0.000676 0.000379 0.000209 0.000114 0.000133

@assert male.2010
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.136855 0.461281 3.126048 13.770339 27.262862 31.754424 29.486766 25.139891 22.281759 21.191735 20.977435 20.707830 19.837068 18.231893 16.052838 13.579271 11.084885 8.771011 6.752799 5.075137 3.733087 2.692717 1.907374 1.328177 0.909880 0.613579 0.407482 0.266597 0.171886 0.109239 0.068448 0.042294 0.025775 0.015495 0.009189 0.005377 0.003104 0.001769 0.000994 0.000552 0.000302 0.000163 0.000190

@assert male.2011
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.001161 0.295778 1.705023 5.320431 17.366866 34.161519 43.186590 42.979079 38.792280 33.876962 29.553792 25.952746 22.787192 19.802667 16.897473 14.098549 11.483581 9.131450 7.093888 5.389069 4.007978 2.921497 2.089182 1.466881 1.011964 0.686357 0.457909 0.300648 0.194344 0.123733 0.077615 0.047984 0.029244 0.017574 0.010416 0.006090 0.003512 0.001999 0.001122 0.000622 0.000340 0.000183 0.000098 0.000116

@assert male.2012
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.021158 1.155698 6.907214 20.470618 40.260517 52.291791 52.148506 46.519931 40.323904 35.344472 31.460620 28.110941 24.864317 21.561895 18.243981 15.044104 12.095019 9.491793 7.280316 5.463812 4.016803 2.895684 2.048690 1.423506 0.971979 0.652517 0.430886 0.279995 0.179111 0.112832 0.070020 0.042817 0.025807 0.015335 0.008985 0.005193 0.002960 0.001664 0.000923 0.000505 0.000273 0.000145 0.000076 0.000096

@assert male.2013
type partition
values 0.000000 0.000000 0.000000 0.000000 0.003295 0.581819 2.379174 10.681362 25.795539 42.257126 55.689469 60.578564 59.130112 54.504702 48.956383 43.387088 37.962579 32.717897 27.697915 22.996939 18.718777 14.945921 11.713331 9.016084 6.818823 5.067727 3.701805 2.658188 1.876626 1.302647 0.889145 0.596854 0.394078 0.255975 0.163610 0.102926 0.063746 0.038878 0.023355 0.013823 0.008061 0.004634 0.002626 0.001467 0.000808 0.000439 0.000235 0.000124 0.000065 0.000093

@assert male.2014
type partition
values 0.000000 0.000000 0.000000 0.000000 0.087402 2.647482 10.784453 22.190545 39.003985 54.768703 63.836960 66.537754 64.657098 60.173386 54.312456 47.763482 40.969360 34.324269 28.116189 22.544224 17.716433 13.664949 10.355576 7.716223 5.655755 4.078421 2.893957 2.020983 1.389141 0.939873 0.625971 0.410424 0.264940 0.168404 0.105418 0.064998 0.039482 0.023631 0.013939 0.008105 0.004645 0.002625 0.001463 0.000804 0.000436 0.000233 0.000123 0.000064 0.000033 0.000058

@assert female.untagged
type partition
values 0.000000 684358.903620 596594.446122 516734.225436 443971.012337 378236.767636 319684.737210 268882.740474 225994.400329 190193.988617 160480.836498 135769.281639 115348.744812 98432.318744 84419.759865 72879.616778 63404.470225 55657.685501 49279.020207 43959.300103 39449.989875 35572.255651 32182.982254 29175.921080 26471.820029 24011.125461 21755.949344 19681.075927 17768.540419 16005.322518 14381.777647 12890.365626 11524.680303 10278.760461 9146.647451 8122.146145 7198.742037 6369.628105 5627.799652 4966.182581 4377.768954 3855.742016 3393.580238 2985.135763 2624.686866 2306.966762 2027.172502 1780.958286 1564.417352 11275.788850

@assert female.2001
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.002218 0.136548 0.781330 1.482472 1.412983 0.843165 0.403622 0.183976 0.083867 0.038613 0.017981 0.008483 0.004064 0.001980 0.000982 0.000496 0.000255 0.000133 0.000070 0.000038 0.000020 0.000011 0.000006 0.000003 0.000002 0.000001 0.000001 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert female.2002
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000025 0.149904 1.125035 2.118411 1.896168 2.478049 2.967795 2.587903 1.861190 1.207645 0.746823 0.453846 0.274990 0.166920 0.101516 0.061777 0.037569 0.022813 0.013826 0.008361 0.005044 0.003035 0.001822 0.001090 0.000650 0.000386 0.000228 0.000134 0.000079 0.000046 0.000027 0.000015 0.000009 0.000005 0.000006

@assert female.2003
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.018640 0.092297 0.553832 0.817028 1.194442 2.173480 3.256897 3.793710 3.928061 3.833912 3.538336 3.072653 2.524032 1.982028 1.502546 1.107545 0.797574 0.562883 0.390176 0.266091 0.178781 0.118474 0.077508 0.050099 0.032013 0.020233 0.012652 0.007829 0.004795 0.002907 0.001744 0.001036 0.000609 0.000354 0.000204 0.000261

@assert female.2004
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.029617 0.443349 7.749139 15.487845 12.284697 9.557401 8.980816 8.193318 6.703963 5.115017 3.837480 2.889892 2.178446 1.630674 1.207047 0.884215 0.643328 0.466641 0.338160 0.244788 0.176663 0.126752 0.090149 0.063400 0.044011 0.030119 0.020306 0.013483 0.008816 0.005677 0.003601 0.002250 0.001385 0.000841 0.000503 0.000297 0.000396

@assert female.2005
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000055 0.724364 13.964062 25.854161 23.022660 20.283186 18.320256 14.917950 11.525692 8.892348 7.003717 5.615028 4.518026 3.598431 2.814332 2.155596 1.617270 1.190054 0.860026 0.611106 0.427342 0.294309 0.199739 0.133653 0.088216 0.057457 0.036941 0.023452 0.014705 0.009108 0.005573 0.003369 0.002013 0.001188 0.000693 0.000399 0.000227 0.000283

@assert female.2006
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.042718 0.649752 7.713746 14.173237 12.755291 12.379934 14.138852 15.974695 16.765203 16.573063 15.815799 14.610194 12.992379 11.100956 9.137365 7.280800 5.644049 4.274217 3.172026 2.312032 1.657581 1.170032 0.813635 0.557627 0.376757 0.251000 0.164915 0.106879 0.068334 0.043107 0.026834 0.016484 0.009995 0.005981 0.003533 0.002060 0.001185 0.000673 0.000834

@assert female.2007
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.049273 2.664146 23.275680 36.060757 33.920052 29.591105 24.145160 18.533475 15.058675 13.899598 13.814987 13.540902 12.556452 10.956389 9.063641 7.175315 5.480452 4.063898 2.939063 2.080004 1.444040 0.985280 0.661642 0.437773 0.285637 0.183914 0.116919 0.073419 0.045553 0.027933 0.016931 0.010145 0.006010 0.003520 0.002038 0.001166 0.000660 0.000369 0.000204 0.000240

@assert female.2008
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.057278 0.586570 11.033070 24.509857 30.773459 30.317541 24.694224 18.115787 13.717143 11.412481 10.222615 9.358333 8.438804 7.367057 6.193032 5.014643 3.921181 2.971268 2.189458 1.573858 1.106601 0.762744 0.516319 0.343756 0.225367 0.145631 0.092825 0.058397 0.036277 0.022260 0.013497 0.008087 0.004789 0.002804 0.001622 0.000928 0.000525 0.000293 0.000162 0.000088 0.000102

@assert female.2009
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.269000 12.854117 41.386834 41.039690 34.795549 26.573569 19.198723 14.881280 13.069654 12.361363 11.687901 10.653283 9.285803 7.756250 6.237832 4.854076 3.670901 2.708424 1.955802 1.385689 0.965012 0.661455 0.446664 0.297351 0.195245 0.126493 0.080881 0.051052 0.031816 0.019579 0.011898 0.007141 0.004233 0.002478 0.001433 0.000819 0.000462 0.000257 0.000141 0.000077 0.000089

@assert female.2010
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.078564 0.377090 2.662357 13.360250 27.677325 32.104328 29.500021 25.341755 23.457185 23.529660 24.108180 24.000006 22.697022 20.307743 17.276071 14.080515 11.078485 8.468298 6.318911 4.619255 3.316415 2.342260 1.628935 1.116180 0.753848 0.501940 0.329542 0.213365 0.136252 0.085828 0.053339 0.032706 0.019790 0.011817 0.006965 0.004052 0.002326 0.001319 0.000738 0.000407 0.000222 0.000120 0.000138

@assert female.2011
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000033 0.202077 1.566984 5.133065 18.150418 36.436390 45.429331 44.350678 39.692557 34.946879 31.063317 27.774985 24.609174 21.334439 17.979346 14.709547 11.695651 9.056531 6.844989 5.059202 3.663140 2.602164 1.815678 1.245574 0.840713 0.558644 0.365635 0.235815 0.149921 0.093986 0.058116 0.035453 0.021342 0.012680 0.007436 0.004305 0.002461 0.001389 0.000774 0.000426 0.000231 0.000124 0.000066 0.000077

@assert female.2012
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.008180 0.893259 6.667623 21.343589 43.149990 55.216818 53.703689 47.506153 41.864087 37.809798 34.558115 31.290873 27.639727 23.659025 19.606245 15.763282 12.329447 9.406922 7.016549 5.124989 3.670988 2.581711 1.784294 1.212748 0.811094 0.534045 0.346314 0.221261 0.139325 0.086490 0.052946 0.031969 0.019044 0.011194 0.006493 0.003718 0.002101 0.001172 0.000645 0.000351 0.000188 0.000100 0.000052 0.000065

@assert female.2013
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000095 0.473932 2.073164 10.320651 26.545402 44.998273 59.900430 64.606027 62.632563 57.929852 52.595472 47.088086 41.366430 35.530262 29.796336 24.416505 19.585131 15.411184 11.914232 9.057635 6.773848 4.982968 3.605277 2.565422 1.795240 1.235442 0.836139 0.556597 0.364490 0.234861 0.148946 0.092995 0.057177 0.034629 0.020665 0.012153 0.007046 0.004027 0.002270 0.001262 0.000692 0.000374 0.000200 0.000105 0.000055 0.000077

@assert female.2014
type partition
values 0.000000 0.000000 0.000000 0.000000 0.034332 2.144583 10.541185 22.879653 41.922103 59.881771 69.839571 72.596711 70.643149 66.022342 59.702538 52.264950 44.299090 36.453301 29.216516 22.883573 17.566637 13.250952 9.838228 7.196826 5.189470 3.688674 2.584502 1.784901 1.214857 0.814793 0.538428 0.350537 0.224831 0.142072 0.088455 0.054268 0.032812 0.019555 0.011489 0.006655 0.003802 0.002142 0.001190 0.000652 0.000353 0.000188 0.000099 0.000051 0.000026 0.000047
)";

} /* namespace testcases */
} /* namespace niwa */

#endif /* TESTMODE */
#endif /* SOURCE_TESTRESOURCES_MODELS_ROSSSEACOMPLEX_H_ */
