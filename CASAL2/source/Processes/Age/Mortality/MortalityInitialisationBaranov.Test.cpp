/**
 * @file MortalityInitialisationBaranov.Test.cpp
 * @author C.Marsh
 * @github Craig44
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "Model/Model.h"
#include "MortalityInitialisationBaranov.h"
#include "Partition/Partition.h"
#include "Processes/Manager.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"


// Namespaces
namespace niwa {
namespace processes {
namespace age {

using niwa::testfixtures::InternalEmptyModel;

const std::string test_cases_simple_model =
R"(
  @model
  start_year 1990
  final_year 2000
  min_age 1
  max_age 8
  age_plus true
  initialisation_phases Fixed init_f
  time_steps step1

  @categories 
  format sex 
  names     male female 
  age_lengths   AL   AL  

  @initialisation_phase Fixed
  type state_category_by_age
  categories male female
  min_age 1
  max_age 8
  table n
  male 1000 900 800 700 600 500 400 700
  female 1000 900 800 700 600 500 400 700
  end_table

  @process 	init_F
  type 		mortality_initialisation_baranov
  categories	*
  selectivities  One=[type=constant; c=1] * 2
  fishing_mortality 0.5


  @time_step step1
  processes Nop

  @process Nop
  type null_process

  @age_length AL
  type none
  length_weight length_weight
  @length_weight length_weight
  type none
)";


const std::string one_year_init_f =
R"(
  @initialisation_phase	init_f
  type					iterative 
  insert_processes step1()=init_F
  years 1
)";

const std::string ten_year_init_f =
R"(
  @initialisation_phase	init_f
  type					iterative 
  insert_processes step1()=init_F
  years 10
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_Initialisation_Mortality_Baranov_one_year) {
  AddConfigurationLine(test_cases_simple_model, __FILE__, 31);
  AddConfigurationLine(one_year_init_f, __FILE__, 136);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);
 // see @initialisation phase Fixed for why these values were seeded
  vector<Double> start_values = {1000, 900, 800, 700, 600 ,500, 400 ,700};

  /*
  ## the equivalent R code for this test
  part = c(1000, 900, 800 ,700 ,600 ,500, 400, 700)
  part * exp(-0.5)
  */
  partition::Category& stock = model_->partition().category("male");
  for (unsigned i = 0; i < start_values.size(); ++i) {
    EXPECT_DOUBLE_EQ(start_values[i] * exp(-0.5), stock.data_[i]) << " with i = " << i;
  }
}

/**
 * Test the init baranov when you apply it 10 years 
 */
TEST_F(InternalEmptyModel, Processes_Initialisation_Mortality_Baranov_ten_year) {
  AddConfigurationLine(test_cases_simple_model, __FILE__, 31);
  AddConfigurationLine(ten_year_init_f, __FILE__, 136);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);
  // see @initialisation phase Fixed for why these values were seeded
  vector<Double> start_values = {1000, 900, 800, 700, 600 ,500, 400 ,700};
  // apply init F for 10 years
  partition::Category& stock = model_->partition().category("male");
  for (unsigned i = 0; i < start_values.size(); ++i) {
    EXPECT_DOUBLE_EQ(start_values[i] * pow(exp(-0.5), 10), stock.data_[i]) << " with i = " << i;
  }
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* TESTMODE */
