
/**
 * @file StateCategoryByAge.Test.Test.cpp
 * @author Craig Marsh
 * @github https://github.com/Zaita
 * @date 30/09/2017
 * @section LICENSE
 *
 * Copyright NIWA Science ©2017 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "StateCategoryByAge.h"

#include <iostream>

#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "Common/DerivedQuantities/Manager.h"

// Namespaces
namespace niwa {
namespace observations {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

const std::string test_cases_simple_model =
R"(
@model
start_year 1990
final_year 2000
min_age 1
max_age 20
plus_group true
initialisation_phases Fixed
time_steps step1

@categories 
format sex 
names     male female 
age_lengths   AL   AL  

@initialisation_phase Fixed
type state_category_by_age
categories male female
min_age 3
max_age 10
table n
male 1000 900 800 700 600 500 400 700
female 1000 900 800 700 600 500 400 700
end_table

@time_step step1
processes Rec M Ageing

@recruitment Rec
type constant
categories *
r0 59000
proportions 0.5 0.5 
age 1

@process Ageing
type ageing
categories *

@mortality M
type constant_rate
m 0.4*2
categories *
time_step_ratio 1
selectivities [type = constant; c = 1]*2

@age_length AL
type schnute
by_length true
time_step_proportions 0.1
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

@derived_quantity SSB
type biomass
categories *
selectivities [type = constant; c = 1]*2
time_step step1

@report DQ
type derived_quantity

)";

/**
 *
 */
TEST_F(InternalEmptyModel, Initialisation_StateCategoryByAge) {
  AddConfigurationLine(test_cases_simple_model, __FILE__, 31);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);
  niwa::DerivedQuantity* dq = model_->managers().derived_quantity()->GetDerivedQuantity("SSB");
  vector<double> expect = {33.51281314437621,  44.084583149802228, 62.038992626827039, 82.775717533653165, 102.9638431530089, 120.79049356072068, 135.57009890088847,  147.29749698571644, 156.30911917665628, 163.06745196537008, 168.04083651251662};
  for (unsigned i = 0; i < expect.size(); ++i) {
    unsigned year = 1990 + i;
    EXPECT_DOUBLE_EQ(expect[i], dq->GetValue(year)) << " for year " << year << " and value " << expect[i];
  }


}

} /* namespace processes */
} /* namespace niwa */


#endif /* TESTMODE */
