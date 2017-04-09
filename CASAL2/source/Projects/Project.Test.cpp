/**
 * @file Project.Test.cpp
 * @author Craig Marsh
 * @github https://github.com/Zaita
 * @date 8/4/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2016 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "Partition/Partition.h"
#include "Model/Model.h"
#include "Estimates/Manager.h"
#include "TestResources/TestCases/CasalComplex1.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

#include "Utilities/RandomNumberGenerator.h"

namespace niwa {
namespace projects {


using niwa::testfixtures::InternalEmptyModel;


const string simple_model =
    R"(
@model
start_year 2000
final_year 2012
projection_final_year 2020
min_age 1
max_age 30
age_plus true
base_weight_units tonnes
initialisation_phases Equilibrium_state
time_steps Sep_Feb Mar_May

@categories 
format stock
names HAK4 
age_lengths age_size 

@initialisation_phase Equilibrium_state
type derived

@time_step Sep_Feb 
processes Recruitment Instantaneou_Mortality
@time_step Mar_May 
processes Instantaneou_Mortality Ageing

@process Recruitment
type recruitment_beverton_holt
categories HAK4
proportions 1 
b0 44000
standardise_ycs_years 2004 2005 2006
ycs_values  1*13
steepness 0.9
ssb SSB
age 1
ycs_years 1999:2011

@process Ageing
type ageing
categories HAK4

@process Instantaneou_Mortality
type mortality_instantaneous
m 0.2
time_step_ratio 0.42 0.58
selectivities One
categories HAK4
table catches
year FishingWest 
2000  1155  
2001  1208
2002  454
2003  497
2004  687
2005  2585
2006  184
2007  270
2008  259
2009  1069
2010  231
2011  822
2012  800
end_table

table method
method    category  selectivity   u_max   time_step     penalty
FishingWest     HAK4      westFSel  0.7   Sep_Feb     none
end_table

@derived_quantity SSB
type biomass
time_step Sep_Feb
categories HAK4
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
time_step_proportions 0.25 0.5
y1 24.5
y2 104.8
tau1 1 
tau2 20 
a 0.131
b 1.70
cv_first 0.1
length_weight Length_Weight 
@length_weight Length_Weight
type basic
units kgs 
a 2.0e-6 
b 3.288

)";

/**
 * LogNormal_test
 */
const string lognormal_project =
R"(
 @project future_ycs
 type lognormal
 parameter process[Recruitment].ycs_values
 years 2009:2019
 mean 0
 sigma 0.8

)";
/**
 *  Test LogNormal @project for estimate of type map and
 */

TEST_F(InternalEmptyModel,Projection_Run_lognormal) {
  utilities::RandomNumberGenerator::Instance().Reset(3445u);
  AddConfigurationLine(simple_model, __FILE__, 31);
  AddConfigurationLine(lognormal_project, __FILE__, 55);
  LoadConfiguration();
  model_->Start(RunMode::kProjection);
  string error_message = "";
  //Double* = model_->objects().FindEstimable("process[Recruitment].ycs_values",error_message);
  //if (!estimate)
  //  LOG_FATAL() << "!estimate";
  //EXPECT_DOUBLE_EQ(estimate->value(), 7.2724038656178385);
//
//  partition::Category& immature_male   = model_->partition().category("immature.male");
//  partition::Category& immature_female = model_->partition().category("immature.female");
//
//  // Verify blank partition for the last time_step
//  for (unsigned i = 0; i < immature_male.data_.size(); ++i)
//    EXPECT_DOUBLE_EQ(0.0, immature_male.data_[i]) << " where i = " << i << "; age = " << i + immature_male.min_age_;
//  for (unsigned i = 0; i < immature_female.data_.size(); ++i)
//    EXPECT_DOUBLE_EQ(0.0, immature_female.data_[i]) << " where i = " << i << "; age = " << i + immature_female.min_age_;
}

const string lognormal_vector =
R"(
@project future_catches
type lognormal
parameter process[Fishing].catches(2008:2012)
mean 10.0944
sigma 1
)";

} /* namespace projects */
} /* namespace niwa */
#endif
