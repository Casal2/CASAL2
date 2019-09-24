/**
 * @file RecruitmentBevertonHolt.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 20/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Processes/Manager.h"
#include "Model/Model.h"
#include "Partition/Partition.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

/**
 *
 */
const std::string test_cases_process_recruitment_bh =
R"(
@model
start_year 1994
final_year 2012
min_age 1
base_weight_units kgs
max_age 12
age_plus t
initialisation_phases iphase1 iphase2
time_steps step_one=[processes=Recruitment] step_two=[processes=Ageing]

@categories
format stage.sex
names immature.male mature.male immature.female mature.female

@initialisation_phase iphase1
years 200

@initialisation_phase iphase2
years 1

@ageing Ageing
categories *

@recruitment Recruitment
type beverton_holt
categories stage=immature
proportions 0.5 0.5
r0 1.6059e+006
b0_initialisation_phase iphase2
age 1
ycs_years 1993:2011
ssb SSB
standardise_ycs_years 1995:2006
ycs_values 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
steepness 0.75

@derived_quantity SSB
type biomass
time_step model.step_one
categories stage=immature
time_step_proportion 1.0
selectivities MaturityMale MaturityFemale

@selectivity MaturityMale
type logistic
a50 11.99
ato95 5.25

@selectivity MaturityFemale
type logistic
a50 16.92
ato95 7.68

@report DQ
type derived_quantity
)";

TEST_F(InternalEmptyModel, Processes_BevertonHolt_Recruitment_AutoSSBOffset) {
  AddConfigurationLine(test_cases_process_recruitment_bh, __FILE__, 72);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male   = model_->partition().category("immature.male");
  EXPECT_DOUBLE_EQ(0.0, male.data_[0]);
  EXPECT_DOUBLE_EQ(808729.94416124187, male.data_[1]);
  EXPECT_DOUBLE_EQ(808432.22560777736, male.data_[2]);
  EXPECT_DOUBLE_EQ(808131.97955769731,  male.data_[3]);
  EXPECT_DOUBLE_EQ(807829.17489105463, male.data_[4]);
  EXPECT_DOUBLE_EQ(807523.7799964858, male.data_[5]);
  EXPECT_DOUBLE_EQ(807215.76276109123, male.data_[6]);
  EXPECT_DOUBLE_EQ(806905.09056026023, male.data_[7]);

  partition::Category& female = model_->partition().category("immature.female");
  EXPECT_DOUBLE_EQ(0.0, female.data_[0]);
  EXPECT_DOUBLE_EQ(808729.94416124187, female.data_[1]);
  EXPECT_DOUBLE_EQ(808432.22560777736, female.data_[2]);
  EXPECT_DOUBLE_EQ(808131.97955769731,  female.data_[3]);
  EXPECT_DOUBLE_EQ(807829.17489105463, female.data_[4]);
  EXPECT_DOUBLE_EQ(807523.7799964858, female.data_[5]);
  EXPECT_DOUBLE_EQ(807215.76276109123, female.data_[6]);
  EXPECT_DOUBLE_EQ(806905.09056026023, female.data_[7]);
}

/**
 *  Test B0 initialsied models
 */
const std::string test_cases_process_recruitment_bh_b0 =
R"(
@model
start_year 1975
final_year 2012
min_age 1
max_age 30
base_weight_units kgs
age_plus true
initialisation_phases iphase1
time_steps step1 step2 step3

@categories
format stock
names stock
age_lengths age_size

@initialisation_phase iphase1
type derived

@time_step step1
processes Recruitment instant_mort

@time_step step2
processes instant_mort

@time_step step3
processes  Ageing instant_mort

@process Recruitment
type recruitment_beverton_holt
categories stock
proportions 1
b0 5.18767e+008
ycs_years 1974:2011
standardise_ycs_years 1975 1976 1977 1978 1979 1980 1981 1982 1983 1984 1985 1986 1987 1988 1989 1990 1991 1992 1993 1994 1995 1996 1997 1998 1999 2000 2001 2002 2003 2004 2005 2006 2007 2008 2009
ycs_values          1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00
steepness 0.9
ssb biomass_t1
age 1

@process Ageing
type ageing
categories stock

@penalty CatchMustBeTaken1
type process
log_scale True
multiplier 1000

@process instant_mort
type mortality_instantaneous
m 0.19
time_step_ratio 0.42 0.25 0.33
selectivities One
categories stock
table catches
year FishingWest FishingEest
1975  80000 111000
1976  152000  336000
1977  74000 1214000
1978  28000 6000
1979  103000  506000
1980  481000  269000
1981  914000  83000
1982  393000  203000
1983  154000  148000
1984  224000  120000
1985  232000  312000
1986  282000  80000
1987  387000  122000
1988  385000  189000
1989  386000  418000
1990  309000  689000
1991  409000  503000
1992  718000  1087000
1993  656000  1996000
1994  368000  2912000
1995  597000  2903000
1996  1353000 2483000
1997  1475000 1820000
1998  1424000 1124000
1999  1169000 3339000
2000  1155000 2130000
2001  1208000 1700000
2002  454000  1058000
2003  497000  718000
2004  687000  1983000
2005  2585000 1434000
2006  184000  255000
2007  270000  683000
2008  259000  901000
2009  1069000 832000
2010  231000  159000
2011  822000  118000
2012  800000  150000
end_table

table method
method   category  selectivity   u_max   time_step   penalty
FishingWest     stock     westFSel  0.7   step1     CatchMustBeTaken1
FishingEest   stock     eastFSel  0.7   step1     CatchMustBeTaken1
end_table


@derived_quantity biomass_t1
type biomass
time_step step1
categories stock
time_step_proportion 0.5
time_step_proportion_method weighted_sum
selectivities MaturationSel

@selectivity MaturationSel
type all_values_bounded
l 2
h 13
v 0.02  0.05  0.13  0.29  0.50  0.70  0.84  0.93  0.97  0.99  0.99  1.00

@selectivity westFSel
type double_normal
mu 6
sigma_l  3
sigma_r 10
alpha 1.0

@selectivity eastFSel
type double_normal
mu 6
sigma_l  3
sigma_r 10
alpha 1.0

@selectivity chatTANSel
type double_normal
mu 6
sigma_l 3
sigma_r 10
alpha 1.0

@selectivity One
type constant
c 1

@age_length age_size
type schnute
by_length true
time_step_proportions 0.25 0.5 0.0
y1 24.5
y2 104.8
tau1 1
tau2 20
a 0.131
b 1.70
cv_first 0.1
length_weight size_weight3

@length_weight size_weight3
type basic
units kgs
a 2.0e-6
b 3.288

@report DQ
type derived_quantity

)";

TEST_F(InternalEmptyModel, Processes_BevertonHolt_Recruitment_B0_initialised) {
  AddConfigurationLine(test_cases_process_recruitment_bh_b0, __FILE__, 72);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& stock   = model_->partition().category("stock");
  EXPECT_DOUBLE_EQ(0.0, stock.data_[0]);
  EXPECT_DOUBLE_EQ(37853202.461915314, stock.data_[1]);
  EXPECT_DOUBLE_EQ(31283397.351866711, stock.data_[2]);
  EXPECT_DOUBLE_EQ(25848294.516795192,  stock.data_[3]);
  EXPECT_DOUBLE_EQ(21342118.067770876, stock.data_[4]);
  EXPECT_DOUBLE_EQ(17614842.936839949, stock.data_[5]);
  EXPECT_DOUBLE_EQ(14534863.966055876, stock.data_[6]);
  EXPECT_DOUBLE_EQ(11994661.788456632, stock.data_[7]);
}


} /* namespace age */
} /* namespace processes */
} /* namespace niwa */


#endif /* TESTMODE */
