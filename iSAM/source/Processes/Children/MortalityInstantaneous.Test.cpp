/**
 * @file MortalityInstantaneous.Test.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 13/08/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "MortalityInstantaneous.h"

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Processes/Manager.h"
#include "Model/Model.h"
#include "Partition/Partition.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"


// Namespaces
namespace niwa {
namespace processes {

using niwa::testfixtures::InternalEmptyModel;

const std::string test_cases_process_mortality_instantaneous =
R"(
@model
start_year 1970 
final_year 2012
min_age 1
max_age 30
age_plus true
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

@process fishing
type mortality_instantaneous
m 0.0798
u_max 0.7
years 1975 1976 1977 1978 1979 1980 1981 1982 1983 1984 1985 1986 1987 1988 1989 1990 1991 1992 1993 1994 1995 1996 1997 1998 1999 2000 2001 2002 2003 2004 2005 2006 2007 2008 2009 2010 2011 2012
categories stock
table catches
fishery 1970:1974 1975 1976 1977 1978 1979 1980 1981 1982 1983 1984 1985 1986 1987 1988 1989 1990 1991 1992 1993 1994 1995 1996 1997 1998 1999 2000 2001 2002 2003 2004 2005 2006 2007 2008 2009 2010 2011 2012
FishingWest 0*4  80000  152000  74000 28000 103000  481000  914000  393000  154000  224000  232000  282000  387000  385000  386000  309000  409000  718000  656000  368000  597000  1353000 1475000 1424000 1169000 1155000 1208000 454000  497000  687000  2585000 184000  270000  259000  1069000 231000  822000  800000
FishingEest 0*4 111000     336000 1214000 6000  506000  269000  83000 203000  148000  120000  312000  80000 122000  189000  418000  689000  503000  1087000 1996000 2912000 2903000 2483000 1820000 1124000 3339000 2130000 1700000 1058000 718000  1983000 1434000 255000  683000  901000  832000  159000  118000  150000                        
end_table
table fisheries
fishery  category selectivity 
FishingWest   stock   westFSel
FishingEest  stock   eastFSel
end_table

@derived_quantity biomass_t1
type biomass
time_step step1
categories stock
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
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_Mortality_Instantaneous_Simple) {
  AddConfigurationLine(test_cases_process_mortality_instantaneous, __FILE__, 31);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kBasic);

  vector<Double> expected = { 0.000000, 4949176.82048178, 4064898.2621518681, 3328684.1087008952, 2706834.6065880726,
      2190888.450324099, 1767721.5580938468, 1425641.5667563954, 1140148.3260357741, 907812.45802352321, 720031.05614045332,
      567731.9296628146, 445605.3260517047, 350563.78174247633, 275339.15723073942, 215262.40630128933, 167136.53823274924,
      129653.50528339949, 101157.68202146454, 79586.351305909513, 63116.075477451588, 50550.346367653998, 41118.131347039118,
      33999.31527315217, 28565.45150964101, 24327.409298919287,20914.675439551294, 18089.274781314001, 15700.519200272849,
      95507.546634746832 };

  partition::Category& stock = Partition::Instance().category("stock");
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected[i], stock.data_[i]) << " with i = " << i;
  }
}

} /* namespace processes */
} /* namespace niwa */
#endif /* TESTMODE */
