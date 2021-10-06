/**
 * @file AgeLength.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 29/01/2018
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestResources/TestFixtures/InternalEmptyModel.h"

#include "../TimeSteps/Manager.h"
#include "AgeLength.h"
#include "Categories/Categories.h"
#include "Partition/Partition.h"

// Namespaces
namespace niwa {
namespace agelengths {
using ::testing::Return;
using ::testing::ReturnRef;
using niwa::testfixtures::InternalEmptyModel;

/**
 *  Test Base test
 */
const std::string test_cases_base =
    R"(
@model
start_year 1975
final_year 1977
min_age 1
max_age 30
base_weight_units kgs
age_plus true
initialisation_phases iphase1
time_steps step1

@categories
format stock
names stock
age_lengths age_size

@initialisation_phase iphase1
type derived

@time_step step1
processes Recruitment instant_mort Ageing


@process Recruitment
type recruitment_beverton_holt
categories stock
proportions 1
b0 5.18767e+008
ycs_years 1974:1976
standardise_ycs_years 1974:1976
ycs_values          1.00 1.00 1.00 
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
time_step_proportions 1
relative_m_by_age One
categories stock
table catches
year FishingWest FishingEest
1975  80000 111000
1976  152000  336000
1977  74000 1214000
end_table

table method
method   category  selectivity   u_max   time_step   penalty
FishingWest     stock     One  0.7   step1     CatchMustBeTaken1
FishingEest   stock     One  0.7   step1     CatchMustBeTaken1
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
length_weight size_weight
distribution none

@length_weight size_weight
type basic
units kgs
a 2.0e-6
b 3.288
)";


/**
 *  change Length weight
 */
const std::string vary_LW_a = R"(
@time_varying l_w_vary
type constant
years 1977
parameter length_weight[size_weight].a
values 3.0e-6
)";

/**
 *  change Age-Length
 */
const std::string vary_age_length = R"(
@time_varying age_length_vary
type constant
years 1977
parameter age_length[age_size].b
values 2.70
)";
/*
* Base line unit test, the following time-varying unit-tests are based on this.
* this has no time-varying functionality
*
*/
TEST_F(InternalEmptyModel, AgeLengthBase) {
  AddConfigurationLine(test_cases_base, __FILE__, 72);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& stock = model_->partition().category("stock");
  // these are from the config @length_weight
  Double a = 2.0e-6;
  Double b = 3.288;
  // these were obtained by running the above model with this report
  /*
  @report agelength
  type age_length
  age_length age_size
  time_step step1
  years 1975:1977
  */
  vector<Double> expected_lengths = {24.5 ,41.5062 ,53.0819 ,61.9345 ,69.0249 ,74.8481 ,79.7065 ,83.8036 ,87.2853 ,90.2614 ,92.8166 ,95.0181 ,96.9204 ,98.5678 ,99.9972 ,101.239 ,102.32 ,103.262 ,104.083 ,104.8 ,105.426 ,105.973 ,106.451 ,106.869 ,107.235 ,107.555 ,107.835 ,108.081 ,108.296 ,108.484 };

  unsigned iter = 0;
  Double length = 0.0;
  for(unsigned age = 1; age <= 30; ++age, ++iter) {
    length = stock.age_length_->mean_length(0, age);
    EXPECT_NEAR(expected_lengths[iter], length, 0.001) << " with i = " << iter + 1;
    EXPECT_NEAR(stock.age_length_->mean_weight(0, age), a*pow(length,b), 0.0001) << " with i = " << iter + 1;
  }
}

/*
* Test that age-length class updates mean_length method when length_weight parameters time-vary
* in this test case, we change the a parameter for the last year and test that it was updated in 
* the partition
*
*/
TEST_F(InternalEmptyModel, AgeLengthBase_time_vary_length_weight_a) {
  AddConfigurationLine(test_cases_base, __FILE__, 72);
  AddConfigurationLine(  vary_LW_a, __FILE__, 9);

  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& stock = model_->partition().category("stock");
  Double a = 3.0e-6; // this varies in the @time-varying block
  Double b = 3.288; // this is  from the config @length_weight
  /*
  @report agelength
  type age_length
  age_length age_size
  time_step step1
  years 1975:1977
  */
  // these were obtained by running the above model with this @report above
  vector<Double> expected_lengths = {24.5 ,41.5062 ,53.0819 ,61.9345 ,69.0249 ,74.8481 ,79.7065 ,83.8036 ,87.2853 ,90.2614 ,92.8166 ,95.0181 ,96.9204 ,98.5678 ,99.9972 ,101.239 ,102.32 ,103.262 ,104.083 ,104.8 ,105.426 ,105.973 ,106.451 ,106.869 ,107.235 ,107.555 ,107.835 ,108.081 ,108.296 ,108.484 };

  unsigned iter = 0;
  Double length = 0.0;
  for(unsigned age = 1; age <= 30; ++age, ++iter) {
    length = stock.age_length_->mean_length(0, age);
    EXPECT_NEAR(expected_lengths[iter], length, 0.001) << " with i = " << iter + 1;
    EXPECT_NEAR(stock.age_length_->mean_weight(0, age), a*pow(length,b), 0.0001) << " with i = " << iter + 1;
  }
}
/*
* This tests a change in the a parameter in the schnute growth model
* R code is imbedded in the test
*
*/
TEST_F(InternalEmptyModel, AgeLengthBase_time_vary_schnute_b) {
  AddConfigurationLine(test_cases_base, __FILE__, 72);
  AddConfigurationLine(vary_age_length, __FILE__, 9);

  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& stock = model_->partition().category("stock");

  /*
  ## R-code used in validation
  #' schnute 
  #' @description general age-length growth formula
  #' @export
  #' @param y1 lenght at reference age t1
  #' @param y2 length at reference age t2
  #' @param t1 reference age for y1
  #' @param t2 reference age for y2
  #' @param a schnute growth param
  #' @param b schnute growth param
  #' @param age age to calculate mean length for
  #' @return return teh mean length at age for the Schnute growth equation
  schnute = function (y1, y2, t1, t2, a, b, age)  {
      if (!(a == 0) & !(b == 0)) {
          (y1^b + (y2^b - y1^b) * ((1 - exp(-a * (age - t1)))/(1 - 
              exp(-a * (t2 - t1)))))^(1/b)
      }
      else if (!(a == 0) & b == 0) {
          (y1 * exp(log(y2/y1) * ((1 - exp(-a * (age - t1)))/(1 - 
              exp(-a * (t2 - t1))))))
      }
      else if (a == 0 & !(b == 0)) {
          ((y1^b + (y2^b - y1^b) * (age - t1)/(t2 - t1))^(1/b))
      }
      else {
          (y1 * exp(log(y2/y1) * (age - t1)/(t2 - t1)))
      }
  }
  ## this is for the time-vary b
  ## note the b is different
  schnute(y1 = 24.5, y2 = 104.8, t1 = 1, t2 = 20, a = 0.131, b = 2.70, age = 1:30)
  */
 vector<Double> result = {24.5 ,52.0340251473708 ,64.1860058251333 ,72.3082102337151 ,78.3355497167895 ,83.0398108578145 ,86.8223909667851 ,89.923953007412 ,92.502318092798 ,94.6675527133431 ,96.4999231975327 ,98.0599651307646 ,99.3945369607013 ,100.540663850798 ,101.528088747333 ,102.38102843561 ,103.119419990345 ,103.759828897551 ,104.316125690044 ,104.8 ,105.221357771273 ,105.588632782334 ,105.909034178847 ,106.188745439964 ,106.433085943764 ,106.646643349282 ,106.83338293475 ,106.996738543083 ,107.13968870298 ,107.26482069564};
  unsigned iter = 0;
  for(unsigned age = 1; age <= 30; ++age, ++iter) {
    EXPECT_NEAR(result[iter], stock.age_length_->mean_length(0, age), 0.00001) << " with i = " << iter + 1;
  }



}

} /* namespace agelengths */
} /* namespace niwa */

#endif
