/**
 * @file MarkovianMovement.Test.cpp
 * @author C.Marsh
 * @github Craig44
 * @date 2023
 * @section LICENSE
 *
 * Copyright NIWA Science 2023 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "Model/Model.h"
#include "MarkovianMovement.h"
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
processes markovian_movement

@selectivity One
type constant
c 1


@age_length AL
type none
length_weight length_weight
@length_weight length_weight
type none
)";

const std::string simple_movement =
R"(

@process markovian_movement
type markovian_movement
from R1 * 4 R2 * 4 R3 * 4 R4 * 4
to R1 R2 R3 R4  R1 R2 R3 R4  R1 R2 R3 R4  R1 R2 R3 R4 
proportions 0.5 0.2 0.2 0.1  0.2 0.5 0.2 0.1 0.2  0.2 0.5 0.1 0.2  0.2  0.1 0.5
selectivities One * 16

)";

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_MarkovianMovement_one_year) {
  AddConfigurationLine(test_cases_simple_model, __FILE__, 31);
  AddConfigurationLine(simple_movement, __FILE__, 31);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  /*
  ## the equivalent R code for this test
  N_age = matrix(c(1000, 900, 800, 700, 600, 500, 400, 700,
   0, 0, 0, 0, 0, 0, 0, 0,
  100, 90, 80, 70, 60, 50, 40, 70,
  2100, 290, 280, 270, 260, 250, 240, 270), nrow = 4, byrow = T)
  move_matrix = matrix(c(0.5, 0.2, 0.2, 0.1,
                         0.2, 0.5, 0.2, 0.1,
                         0.2, 0.2, 0.5, 0.1,
                         0.2, 0.2, 0.1, 0.5), nrow = 4, byrow = T)
  t(t(N_age) %*% move_matrix)
  */
  // see @initialisation phase Fixed for why these values were seeded
   vector<Double> R1_values = {940, 526, 472, 418, 364, 310, 256, 418};
   vector<Double> R2_values = {640, 256, 232, 208, 184, 160, 136, 208};
   vector<Double> R3_values = {460, 254, 228, 202, 176, 150, 124, 202};
   vector<Double> R4_values = {1160, 244, 228, 212, 196, 180, 164, 212};

  partition::Category& R1_n_age = model_->partition().category("R1");
  for (unsigned i = 0; i < R1_n_age.data_.size(); ++i) {
    EXPECT_NEAR(R1_values[i], R1_n_age.data_[i], 0.001) << " with i = " << i;
  }
  partition::Category& R2_n_age = model_->partition().category("R2");
  for (unsigned i = 0; i < R2_n_age.data_.size(); ++i) {
    EXPECT_NEAR(R2_values[i], R2_n_age.data_[i], 0.001) << " with i = " << i;
  }
  partition::Category& R3_n_age = model_->partition().category("R3");
  for (unsigned i = 0; i < R3_n_age.data_.size(); ++i) {
    EXPECT_NEAR(R3_values[i], R3_n_age.data_[i], 0.001) << " with i = " << i;
  }
  partition::Category& R4_n_age = model_->partition().category("R4");
  for (unsigned i = 0; i < R4_n_age.data_.size(); ++i) {
    EXPECT_NEAR(R4_values[i], R4_n_age.data_[i], 0.001) << " with i = " << i;
  }

}


const std::string simple_movement_with_error =
R"(
@process markovian_movement
type markovian_movement
from R1 * 4 R2 * 4 R3 * 4 R4 * 4
to R1 R2 R3 R4  R1 R2 R3 R4  R1 R2 R3 R4  R1 R2 R3 R4 
proportions 0.6 0.2 0.2 0.1  0.2 0.5 0.2 0.1 0.2  0.2 0.5 0.1 0.2  0.2  0.1 0.5
selectivities One * 16
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_MarkovianMovement_error_sum_to_one) {
  AddConfigurationLine(test_cases_simple_model, __FILE__, 31);
  AddConfigurationLine(simple_movement_with_error, __FILE__, 31);
  LoadConfiguration();

  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);

}


} /* namespace age */
} /* namespace processes */
} /* namespace niwa */
#endif /* TESTMODE */
