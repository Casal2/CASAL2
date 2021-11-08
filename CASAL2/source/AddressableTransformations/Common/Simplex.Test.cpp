/**
 * @file Simplex.Test.cpp
 * @author C.Marsh
 * @date 5/11/2021
 * @section LICENSE
 *
 * Copyright NIWA Science �2016 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "../../TestResources/Models/TwoSexNoEstimates.h"
#include "../../TestResources/TestFixtures/InternalEmptyModel.h"
#include "../../BaseClasses/Object.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"
#include "../../Model/Objects.Mock.h"
#include "../../ObjectiveFunction/ObjectiveFunction.h"

#include "../../TestResources/MockClasses/Model.h"

namespace niwa {
namespace addressabletransformations {
using niwa::testfixtures::InternalEmptyModel;


/*
* This first set of tests check bounds sanity checks and also
* checks the simplex work on string maps using process[Recruitment].proportions parameter must sum = 1
*/
const string transform_prop_with_estimate =
R"(
  @parameter_transformation simp_prop
  type simplex
  parameters process[Recruitment].proportions
  sum_to_one true
  prior_applies_to_restored_parameters true

  @estimate prop_simplex
  type uniform
  parameter parameter_transformation[simp_prop].simplex
  lower_bound -10 -10
  upper_bound 10 10
)";
const string transform_prop_with_estimate_expect_true =
R"(
  @parameter_transformation simp_prop
  type simplex
  parameters process[Recruitment].proportions
  sum_to_one true
  prior_applies_to_restored_parameters true

  @estimate prop_simplex
  type uniform
  parameter parameter_transformation[simp_prop].simplex
  lower_bound -10 
  upper_bound 10
)";
const string transform_prop_with_estimate_expect_fail =
R"(
  @parameter_transformation simp_prop
  type simplex
  parameters process[Recruitment].proportions
  sum_to_one true
  prior_applies_to_restored_parameters false

  @estimate prop_simplex
  type uniform
  parameter parameter_transformation[simp_prop].simplex
  lower_bound -10 -10
  upper_bound 10 10
)";
const string transform_propr_expect_prior_false =
R"(
  @parameter_transformation simp_prop
  type simplex
  parameters process[Recruitment].proportions
  sum_to_one true
  prior_applies_to_restored_parameters false

  @estimate prop_simplex
  type uniform
  parameter parameter_transformation[simp_prop].simplex
  lower_bound -10 
  upper_bound 10 
)";
/**
 * if prior_applies_to_restored_parameters == true
 * users need to supply n bounds, if false they need to specify n-1 bounds
 */
TEST_F(InternalEmptyModel, AddressableTransformations_simplex_fail_estimate_bounds_prior_false) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(transform_prop_with_estimate_expect_fail, __FILE__, 51);
  LoadConfiguration();
  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);
}
/**
 * if prior_applies_to_restored_parameters == true
 * users need to supply n bounds, if false they need to specify n-1 bounds
 */
TEST_F(InternalEmptyModel, AddressableTransformations_simplex_fail_estimate_bounds_prior_true) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(transform_prop_with_estimate_expect_true, __FILE__, 51);
  LoadConfiguration();
  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);
}
/**
 *
 */
TEST_F(InternalEmptyModel, AddressableTransformations_simplex_with_jacobian) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(transform_prop_with_estimate, __FILE__, 51);
  LoadConfiguration();
  EXPECT_NO_THROW(model_->Start(RunMode::kBasic));
}
/**
 *
 */
TEST_F(InternalEmptyModel, AddressableTransformations_simplex_with_no_jacobian) {
  AddConfigurationLine(testresources::models::two_sex_no_estimates, "TestResources/Models/TwoSexNoEstimates.h", 28);
  AddConfigurationLine(transform_propr_expect_prior_false, __FILE__, 51);
  LoadConfiguration();
  EXPECT_NO_THROW(model_->Start(RunMode::kBasic));
}
/*
* Basic model to validate YCS transformation
*/
const string simple_model_with_ycs =
R"(
@model
start_year 1975
final_year 2012
min_age 1
max_age 30
age_plus true
base_weight_units tonnes
initialisation_phases iphase1
time_steps step1

@categories
format stock
names stock
age_lengths age_size

@initialisation_phase iphase1
type derived

@time_step step1
processes Recruitment Ageing


@process Recruitment
type recruitment_beverton_holt
categories stock
proportions 1
b0 75000
ycs_values   1.3464 1.3797 1.4091 1.4315 1.4447 1.4468 1.4345 1.4047 1.3708 1.3502 0.5748 0.6419 0.7007 0.5146 0.7098 0.5994 0.9078 1.3517 1.0249 0.8798 0.7981 0.6579 0.4325 0.4085 0.2981 0.4483 0.2961 0.4426 1.2456 0.7079 0.5855 0.2521 0.4138 0.3496 0.3109 0.3503 4.0392 4.0392
ycs_years 1974:2011
steepness 0.9
ssb biomass_t1
age 1

@process Ageing
type ageing
categories stock

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
v 0.02	0.05	0.13	0.29	0.50	0.70	0.84	0.93	0.97	0.99	0.99	1.00

@selectivity One
type constant
c 1

@age_length age_size
type schnute
by_length true
time_step_proportions 0.0
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

@parameter_transformation simplex_ycs
type simplex
parameters process[Recruitment].ycs_values
sum_to_one false
prior_applies_to_restored_parameters true

@estimate ycs_simp
type uniform
parameter parameter_transformation[simplex_ycs].simplex
lower_bound -10*38
upper_bound 10*38

)";
/**
 *
 */
TEST_F(InternalEmptyModel, AddressableTransformations_simplex_validate_simplex_calculation) {
  AddConfigurationLine(simple_model_with_ycs, __FILE__, 51);
  LoadConfiguration();
  EXPECT_NO_THROW(model_->Start(RunMode::kBasic));
  vector<Double>*  simplex = model_->objects().GetAddressableVector("parameter_transformation[simplex_ycs].simplex");
  vector<Double> result = {0.3068, 0.3422, 0.3759, 0.4059, 0.4308, 0.4493, 0.4587, 0.4562, 0.4506, 0.4552, -0.4109, -0.3105, -0.2308, -0.5567, -0.2442, -0.4285, -0.014, 0.4078, 0.1384, -0.0151, -0.1188, -0.3286, -0.7825, -0.8793, -1.2468, -0.8848, -1.3637, -1.0216, 0.0146, -0.6021, -0.8672, -1.8379, -1.48, -1.8324, -2.2012, -2.4449, 0};
  // This result was generated against the following R-code
  // Assuming you copy the config about and run casal2 -r > run.out
  // With the added @reports
  /*
  @report param_trans
  type parameter_transformations
  @report recr
  type process
  process Recruitment
  @report obj
  type objective_function
  */
  // library(Casal2)
  // cas2 = extract.mpd(file = "run.out", path = "testmodel")
  // Then those results were generated as
  // write.table(x = round(t(simplex(input_ycs_vals, sum_to_one  = F) ), 4), row.names = F, col.names = F, quote = F, "result_values.txt", sep = ", ")
  EXPECT_EQ((*simplex).size(), result.size() + 1);

  for(unsigned i = 0; i < result.size(); ++i) {
    EXPECT_NEAR(result[i], (*simplex)[i], 1e-3) << " with i = " << i + 1;
  }
  // Validate the Jacobian Calculation
  niwa::ObjectiveFunction& obj_function = model_->objective_function();
  const vector<objective::Score>& score_list = obj_function.score_list();
  for (objective::Score score : score_list) {
    if(score.label_ == "jacobian->simplex_ycs") {
      EXPECT_NEAR(0.217844, score.score_, 0.001) << " with = " << score.label_; // this is from running 
    }
  }
}




} /* namespace estimatetransformations */
} /* namespace niwa */
#endif
