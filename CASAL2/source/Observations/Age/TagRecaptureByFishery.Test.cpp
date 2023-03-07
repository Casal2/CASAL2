/**
 * @file TagRecaptureByFishery.Test.cpp
 * @author Craig Marsh
 * @github https://github.com/Craig44
 * @date 2023
 * @section LICENSE
 *
 * Copyright NIWA Science 2023 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "TagRecaptureByFishery.h"

#include <iostream>

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Observations/Manager.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace age {

using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

const std::string test_cases_observation_simple_tag_recapture_by_fishery =
    R"(
@model
start_year 2000
final_year 2000
min_age 1
max_age 8
age_plus true
initialisation_phases Fixed
time_steps step1

@categories 
format sex 
names     R1 R2 R3 R4 
age_lengths   AL*4  

@initialisation_phase Fixed
type state_category_by_age
categories R1 R2 R3 R4 
min_age 1
max_age 8
table n
R1 1000 900 800 700 600 500 400 700
R2 0 0 0 0 0 0 0 0
R3 100 90 80 70 60 50 40 70
R4 2100 290 280 270 260 250 240 270
end_table

@time_step step1
processes markovian_movement fishing

@process fishing
type mortality_instantaneous
time_step_proportions 1
m 0
relative_m_by_age One*4
categories *
biomass false
table catches
year Fishing
2000 10120
end_table

table method
method  category selectivity u_max time_step penalty
Fishing   format=*   One 1 step1 none
end_table

@selectivity One
type constant
c 1


@age_length AL
type none
length_weight length_weight
@length_weight length_weight
type none


@process markovian_movement
type markovian_movement
from R1 * 4 R2 * 4 R3 * 4 R4 * 4
to R1 R2 R3 R4  R1 R2 R3 R4  R1 R2 R3 R4  R1 R2 R3 R4 
proportions 0.5 0.2 0.2 0.1  0.2 0.5 0.2 0.1 0.2  0.2 0.5 0.1 0.2  0.2  0.1 0.5
selectivities One * 16

@observation tag_recapture_by_fishing
years 2000
type tag_recapture_by_fishery
tagged_categories R1 R2 R3 R4 
likelihood poisson
time_step step1
mortality_process fishing
method_of_removal Fishing
reporting_rate 1.0
table recaptured
2000 10120
end_table


@report partition
type partition
time_step step1


@report tag_recapture_by_fishing
type observation
observation tag_recapture_by_fishing

@report fishing
type process
process fishing

)";

/**
 *
 */
TEST_F(InternalEmptyModel, Observation_TagRecapture_By_fishery) {
  AddConfigurationLine(test_cases_observation_simple_tag_recapture_by_fishery, __FILE__, 31);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = model_->objective_function();
  EXPECT_NEAR(5.53008, obj_function.score(), 0.001);

  Observation* observation = model_->managers()->observation()->GetObservation("tag_recapture_by_fishing");

  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  ASSERT_EQ(1, comparisons.size());

  unsigned year = 2000;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(1u, comparisons[year].size());
  EXPECT_EQ("tag_recapture_by_fishing", comparisons[year][0].category_);
  EXPECT_DOUBLE_EQ(10120, comparisons[year][0].error_value_);
  EXPECT_DOUBLE_EQ(10120, comparisons[year][0].expected_);
  EXPECT_DOUBLE_EQ(10120, comparisons[year][0].observed_);
  EXPECT_NEAR(5.53008, comparisons[year][0].score_, 0.001); // -1.0 * dpois(10120, 10120, log = T)

}

}  // namespace age
} /* namespace niwa */

#endif /* TESTMODE */
