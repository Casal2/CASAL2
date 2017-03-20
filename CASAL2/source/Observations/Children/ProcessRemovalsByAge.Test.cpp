/**
 * @file ProcessRemovalsByAge.Test.cpp
 * @author Craig Marsh
 * @github https://github.com/Zaita
 * @date 28/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "ProcessRemovalsByAge.h"

#include <iostream>

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Observations/Manager.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace observations {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

const std::string test_cases_observation_process_removals_by_age_single =
R"(
@model
start_year 1990 
final_year 2012
min_age 1
max_age 30
age_plus true
base_weight_units kgs
initialisation_phases iphase1
time_steps init step1 step2 step3

@categories
format stock 
names stock
age_lengths age_size 

@initialisation_phase iphase1
type iterative
years 100

@time_step init  
processes [type=nop]

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
r0 3.04838e+006  
standardise_ycs_years 1989 1990 1991 1992 1993 1994 1995 1996 1997 1998 1999 2000 2001 2002 2003 2004 2005 2006 2007 2008 2009 2010 2011
ycs_values            1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00
steepness 0.9
ssb biomass_t1
age 1
ycs_years 1989:2011

@process Ageing
type ageing
categories stock

@process instant_mort
type mortality_instantaneous
m 0.19
time_step_ratio 0.42 0.25 0.33
selectivities One
categories stock
table catches
year FishingWest FishingEest
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
method       category  selectivity u_max   time_step penalty
FishingWest   stock     westFSel    0.7     step1     none
FishingEest   stock     eastFSel    0.7     step1     none
end_table


@derived_quantity biomass_t1
type biomass
time_step step1
categories stock
time_step_proportion 1.0
selectivities MaturationSel

@selectivity One
type constant
c 1

@selectivity MaturationSel
type all_values_bounded
l 2
h 13
v 0.02  0.05  0.13  0.29  0.50  0.70  0.84  0.93  0.97  0.99  0.99  1.00

@selectivity westFSel 
type double_normal
mu 10
sigma_l 9.33
sigma_r 14.5
alpha 1.0

@selectivity eastFSel
type double_normal
mu 6
sigma_l 3
sigma_r 10
alpha 1.0

@selectivity chatTANSel 
type double_normal
mu 21
sigma_l 17
sigma_r 7
alpha 1.0

@age_length age_size
type schnute
by_length true
time_step_proportions 0.0 0.25 0.5 0.0
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

@observation observation
type process_removals_by_age
likelihood multinomial
time_step step1
mortality_instantaneous_process instant_mort
categories stock
method_of_removal FishingEest
min_age 4
max_age 21
age_plus true
years 1992 1995 1997 1998 2001 2004 2007

table obs
1992    0.0280  0.0241  0.0979  0.0570  0.1119  0.0708  0.1301  0.0906  0.1035  0.0960  0.0431  0.0715  0.0245  0.0334  0.0095  0.0076  0.0003  0.0003
1995    0.0973  0.0744  0.0468  0.1406  0.0826  0.0199  0.0596  0.0579  0.1469  0.0672  0.1135  0.0335  0.0100  0.0180  0.0252  0.0011  0.0054  0.0001
1997    0.1290  0.1608  0.1319  0.0750  0.1510  0.0469  0.0342  0.0383  0.0366  0.0481  0.0563  0.0036  0.0048  0.0207  0.0001  0.0442  0.0005  0.0180
1998    0.0346  0.0816  0.0895  0.1034  0.1289  0.0780  0.0824  0.0659  0.0782  0.0415  0.0505  0.0163  0.0315  0.0360  0.0224  0.0136  0.0291  0.0168
2001    0.0212  0.0348  0.0389  0.1210  0.1385  0.1401  0.1279  0.1272  0.0762  0.0457  0.0308  0.0298  0.0165  0.0159  0.0099  0.0088  0.0035  0.0133
2004    0.0065  0.0115  0.0156  0.0772  0.0858  0.1480  0.1419  0.1247  0.1171  0.1057  0.0379  0.0322  0.0337  0.0157  0.0085  0.0098  0.0055  0.0228
2007    0.0277  0.0732  0.1025  0.1159  0.0942  0.1308  0.0977  0.1001  0.0701  0.0463  0.0455  0.0439  0.0103  0.0246  0.0119  0.0030  0.0001  0.0021
end_table

table error_values
1992 19
1995 18
1997 15
1998 23
2001 53
2004 43
2007 42
end_table    
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Observation_Process_Removals_By_Age_for_fishery_Single) {
  AddConfigurationLine(test_cases_observation_process_removals_by_age_single, __FILE__, 31);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_DOUBLE_EQ(243.66246977233092, obj_function.score());

  Observation* observation = model_->managers().observation()->GetObservation("observation");

  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  ASSERT_EQ(7u, comparisons.size());

  unsigned year = 1997;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(18u, comparisons[year].size());
  EXPECT_EQ("stock",                      comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(15,                    comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(0.19433803070960703,   comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(0.129,                 comparisons[year][0].observed_);
  EXPECT_DOUBLE_EQ(3.8038399067647219,    comparisons[year][0].score_);

  EXPECT_EQ("stock",                      comparisons[year][1].category_);
  EXPECT_DOUBLE_EQ(15,                    comparisons[year][1].error_value_);
  EXPECT_DOUBLE_EQ(0.18541336567128372,   comparisons[year][1].expected_);
  EXPECT_DOUBLE_EQ(0.1608,                comparisons[year][1].observed_);
  EXPECT_DOUBLE_EQ(5.1698115319248554,    comparisons[year][1].score_);

  EXPECT_EQ("stock",                      comparisons[year][2].category_);
  EXPECT_DOUBLE_EQ(15,                    comparisons[year][2].error_value_);
  EXPECT_DOUBLE_EQ(0.14955362757708424,   comparisons[year][2].expected_);
  EXPECT_DOUBLE_EQ(0.13189999999999999,   comparisons[year][2].observed_);
  EXPECT_DOUBLE_EQ(4.4327471728877041,    comparisons[year][2].score_);

  EXPECT_EQ("stock",                      comparisons[year][3].category_);
  EXPECT_DOUBLE_EQ(15,                    comparisons[year][3].error_value_);
  EXPECT_DOUBLE_EQ(0.1120841753156267,   comparisons[year][3].expected_);
  EXPECT_DOUBLE_EQ(0.074999999999999997,  comparisons[year][3].observed_);
  EXPECT_DOUBLE_EQ(2.519828116764669,     comparisons[year][3].score_);

  EXPECT_EQ("stock",                      comparisons[year][4].category_);
  EXPECT_DOUBLE_EQ(15,                    comparisons[year][4].error_value_);
  EXPECT_DOUBLE_EQ(0.084672698040351033,  comparisons[year][4].expected_);
  EXPECT_DOUBLE_EQ(0.151,                 comparisons[year][4].observed_);
  EXPECT_DOUBLE_EQ(6.5432962807430544,    comparisons[year][4].score_);
}

} /* namespace processes */
} /* namespace niwa */


#endif /* TESTMODE */
