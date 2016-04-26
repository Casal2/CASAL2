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
#include "TestResources/Models/CasalComplex1.h"

// Namespaces
namespace niwa {
namespace processes {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_BevertonHolt_Recruitment) {
  AddConfigurationLine(testresources::models::test_cases_models_casal_complex_1, "TestResources/Models/CasalComplex1.h", 32);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male   = model_->partition().category("male");
  EXPECT_DOUBLE_EQ(0.0, male.data_[0]);
  EXPECT_DOUBLE_EQ(2499927.5474240803, male.data_[1]);
  EXPECT_DOUBLE_EQ(2046756.2758644461, male.data_[2]);

  EXPECT_DOUBLE_EQ(151391.67827268399, male.data_[15]);
  EXPECT_DOUBLE_EQ(123889.51433539754, male.data_[16]);
  EXPECT_DOUBLE_EQ(101388.98722028485, male.data_[17]);

  EXPECT_DOUBLE_EQ(45509.118554179389, male.data_[21]);
  EXPECT_DOUBLE_EQ(37254.617197639338, male.data_[22]);
  EXPECT_DOUBLE_EQ(168174.98846106316, male.data_[23]);

  partition::Category& female = model_->partition().category("female");
  EXPECT_DOUBLE_EQ(0.0, male.data_[0]);
  EXPECT_DOUBLE_EQ(2499927.5474240803, female.data_[1]);
  EXPECT_DOUBLE_EQ(2046760.4571484025, female.data_[2]);

  EXPECT_DOUBLE_EQ(151581.19489755956, female.data_[15]);
  EXPECT_DOUBLE_EQ(124062.47306900102, female.data_[16]);
  EXPECT_DOUBLE_EQ(101543.48455296629, female.data_[17]);

  EXPECT_DOUBLE_EQ(45592.839274420774, female.data_[21]);
  EXPECT_DOUBLE_EQ(37324.680701432851, female.data_[22]);
  EXPECT_DOUBLE_EQ(168519.01800652978, female.data_[23]);
}

/**
 *
 */
const std::string test_cases_process_recruitment_bh =
R"(
@model
start_year 1994
final_year 2012
min_age 1
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
b0_intialisation_phase iphase2
age 1
ssb SSB
standardise_ycs_years 1995:2006
ycs_values 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
steepness 0.75

@derived_quantity SSB
type biomass
time_step model.step_one
categories stage=immature
selectivities MaturityMale MaturityFemale 

@selectivity MaturityMale
type logistic 
a50 11.99
ato95 5.25

@selectivity MaturityFemale
type logistic 
a50 16.92
ato95 7.68
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

} /* namespace processes */
} /* namespace niwa */


#endif /* TESTMODE */
