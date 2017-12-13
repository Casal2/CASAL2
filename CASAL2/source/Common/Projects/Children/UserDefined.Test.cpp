/**
 * @file UserDefined.Test.cpp
 * @author C.Marsh
 * @date 21/6/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 */
#ifdef TESTMODE

// Headers
#include "UserDefined.h"

#include <iostream>

#include "Common/ObjectiveFunction/ObjectiveFunction.h"
#include "Common/Projects/Manager.h"
#include "Common/DerivedQuantities/Manager.h"
#include "Common/Model/Model.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace projects {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;


/*
 * Model
*/
const std::string test_cases_mode_with_mortality_instantaneous =
R"(
@model
start_year 1970 
final_year 2012
projection_final_year 2015
min_age 1
max_age 30
plus_group true
base_weight_units kgs
initialisation_phases iphase1
time_steps step1 step2 step3

@categories
format stock
names stock
age_lengths age_size 

@initialisation_phase iphase1
type iterative
years 100

@time_step step1 
processes Recruitment fishing

@time_step step2
processes M 

@time_step step3
processes M Ageing 

@process Recruitment
type recruitment_beverton_holt
categories stock
proportions 1 
r0 6065920
ycs_years 1969:2011
standardise_ycs_years 1969 1970 1971 1972 1973 1974 1975 1976 1977 1978 1979 1980 1981 1982 1983 1984 1985 1986 1987 1988 1989 1990 1991 1992 1993 1994 1995 1996 1997 1998 1999 2000 2001 2002 2003 2004 2005 2006 2007 2008 2009 2010 2011
ycs_values       1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00 1.00
steepness 0.9
ssb biomass_t1
age 1
ssb_offset 1

@process Ageing
type ageing
categories stock

@process M
type mortality_constant_rate
categories stock
M 0.19
selectivities One
time_step_ratio 0.25 0.33

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
mu 6
sigma_l 3
sigma_r 10
alpha 1.0

@selectivity eastFSel
type double_normal
mu 6
sigma_l 3
sigma_r 10
alpha 1.0

@selectivity chatTANSel
type double_normal
mu 6
sigma_l 3
sigma_r 10
alpha 1.0

@age_length age_size
type von_bertalanffy
by_length True
k  0.278
t0 -0.21
Linf 88.0
cv_first 0.2
length_weight size_weight3

@length_weight size_weight3
type basic
units kgs 
a 2.0e-6
b 3.288


@process fishing
type mortality_instantaneous
m 0.0798
selectivities One
categories stock
table catches
year FishingWest FishingEest
1975 80000 111000                                                            
1976 152000 336000
1977 74000 1214000
1978 28000 6000
1979 103000 506000
1980 481000 269000
1981 914000 83000
1982 393000 203000
1983 154000 148000
1984 224000 120000
1985 232000 312000
1986 282000 80000
1987 387000 122000
1988 385000 189000
1989 386000 418000
1990 309000 689000
1991 409000 503000
1992 718000 1087000
1993 656000 1996000
1994 368000 2912000
1995 597000 2903000
1996 1353000 2483000
1997 1475000 1820000
1998 1424000 1124000
1999 1169000 3339000
2000 1155000 2130000
2001 1208000 1700000
2002 454000 1058000
2003 497000 718000
2004 687000 1983000
2005 2585000 1434000
2006 184000 255000
2007 270000 683000
2008 259000 901000
2009 1069000 832000
2010 231000 159000
2011 822000 118000
2014 800000 150000
end_table

table method
method  category selectivity u_max time_step penalty
FishingWest   stock   westFSel 0.7 step1 none
FishingEest  stock   eastFSel 0.7 step1 none
end_table

@report DQ
type derived_quantity

)";

/*
 * YCS projection block
*/
const string constant_ycs =
R"(
@project future_ycs
type constant
parameter process[Recruitment].ycs_values
years 2012:2014
values 0.478482 0.640663 0.640091
)";

const string Catches_2013 =
R"(
	@project western_catch_2013
	type user_defined
	parameter process[fishing].method_FishingWest
	years 2013
	equation if(derived_quantity[biomass_t1].values{2012} / process[Recruitment].b0 <= 0.1, 0.0, if(derived_quantity[biomass_t1].values{2012} / process[Recruitment].b0 > 0.1 && derived_quantity[biomass_t1].values{2012} / process[Recruitment].b0 < 0.2, derived_quantity[biomass_t1].values{2012} * derived_quantity[biomass_t1].values{2012} / process[Recruitment].b0,derived_quantity[biomass_t1].values{2012} * 0.2))
)";

const string Catches_2014 =
R"(
	@project western_catch_2014
	type user_defined
	parameter process[fishing].method_FishingWest
	years 2014
	equation if(derived_quantity[biomass_t1].values{2013} / process[Recruitment].b0 <= 0.1, 0.0, if(derived_quantity[biomass_t1].values{2013} / process[Recruitment].b0 > 0.1 && derived_quantity[biomass_t1].values{2013} / process[Recruitment].b0 < 0.2, derived_quantity[biomass_t1].values{2013} * derived_quantity[biomass_t1].values{2013} / process[Recruitment].b0,derived_quantity[biomass_t1].values{2013} * 0.2))
)";

const string Catches_2015 =
R"(
	@project western_catch_2015
	type user_defined
	parameter process[fishing].method_FishingWest
	years 2015
	equation if(derived_quantity[biomass_t1].values{2014} / process[Recruitment].b0 <= 0.1, 0.0, if(derived_quantity[biomass_t1].values{2014} / process[Recruitment].b0 > 0.1 && derived_quantity[biomass_t1].values{2014} / process[Recruitment].b0 < 0.2, derived_quantity[biomass_t1].values{2014} * derived_quantity[biomass_t1].values{2014} / process[Recruitment].b0,derived_quantity[biomass_t1].values{2014} * 0.2))
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Projects_UserDefined_Catches) {

	AddConfigurationLine(test_cases_mode_with_mortality_instantaneous, __FILE__, 358);
	AddConfigurationLine(constant_ycs, __FILE__, 35);
	AddConfigurationLine(Catches_2013, __FILE__, 65);
	AddConfigurationLine(Catches_2014, __FILE__, 34);
	AddConfigurationLine(Catches_2015, __FILE__, 90);

	LoadConfiguration();
	model_->Start(RunMode::kProjection);

	Project* project_2015 = model_->managers().project()->GetProject("western_catch_2015");
	if(!project_2015)
		LOG_FATAL() << "!project_2015";

	Project* project_2014 = model_->managers().project()->GetProject("western_catch_2014");
	if(!project_2014)
		LOG_FATAL() << "!project_2014";

	Project* project_2013 = model_->managers().project()->GetProject("western_catch_2013");
	if(!project_2013)
		LOG_FATAL() << "!project_2013";

  DerivedQuantity* dq = model_->managers().derived_quantity()->GetDerivedQuantity("biomass_t1");
	if(!dq)
		LOG_FATAL() << "!dq";
	// test the values have changed
	map<unsigned,Double>& values_15 = project_2015->projected_parameters();
	map<unsigned,Double>& values_14 = project_2014->projected_parameters();
	map<unsigned,Double>& values_13 = project_2013->projected_parameters();

	EXPECT_DOUBLE_EQ(6287849.7554903105, values_15[2015]);
	EXPECT_DOUBLE_EQ(6985248.6047034459, values_14[2014]);
	EXPECT_DOUBLE_EQ(7674997.1589622563, values_13[2013]);

  vector<double> Expect = {34926243.023517229,31439248.777451552,28200217.436245844};
	for (unsigned i = 0; i < 3; ++i) {
    unsigned year = 2013 + i;
    EXPECT_DOUBLE_EQ(Expect[i], dq->GetValue(year)) << " for year " << year << " and value " << Expect[i];
  }
}


} /* namespace projects */
} /* namespace niwa */

#endif /* ifdef TESTMODE */
