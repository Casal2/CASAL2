/**
 * @file TagByLength.Test.cpp
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 26/07/2019
 * @section LICENSE
 *
 * Copyright NIWA Science �2019 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file contains unit tests for the TagByLength process
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "Model/Model.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Partition/Partition.h"
#include "Processes/Manager.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace processes {
namespace age {

using niwa::testfixtures::InternalEmptyModel;
using std::cout;
using std::endl;

const std::string test_model_block_process_tag_by_length =
    R"(
@model
start_year 2001
final_year 2005
min_age 1
max_age 20
age_plus t
length_bins 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76  77  78  79  80
length_plus false
initialisation_phases Equilibrium_state
time_steps step1 step2
)";

/*
 * Simple model excluding tag process that I will use to generate test cases.
 * Load this after the model block other wise you will get an error
 */
const std::string test_remaining_model_process_tag_by_length =
    R"(

@categories
format tag
names untagged tagged
age_lengths age_size age_size

@initialisation_phase Equilibrium_state
type derived

@time_step step1
processes  Ageing Recruitment Tagging

@time_step step2
processes Mortality

@ageing Ageing
categories *

@Recruitment Recruitment
type constant
categories untagged tagged
proportions 1.0 0.0
R0 997386
age 1

@process Mortality
type mortality_constant_rate
categories *
m 0.14
relative_m_by_age [type=constant; c=1] [type=constant; c=1]


@age_length age_size
type von_bertalanffy
compatibility_option casal2
length_weight Length_weight
k 0.07690298
t0 -2.17092542
linf 64.43335640
time_step_proportions 0 0
cv_first 0.2

@length_weight Length_weight
type basic
units tonnes
a 4.467e-08
b 2.793
)";

/*
 * A simple tag by length process
 */
const std::string simple_tag =
    R"(
@tag Tagging
type by_length
years 2002:2004
from untagged
to tagged
selectivities [type=constant; c=0.15]
penalty none
n 8190 6782 12046
table proportions
year 10 11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63  64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79
2002 0  0 0 0 0 0 0 0 0 0 0.00010547 0.00005361 0.00043287 0.00029502 0.00140492 0.06609672 0.08515057 0.09074466 0.08993033 0.07277446 0.06806146 0.06183810 0.05511917 0.04408316 0.03674436 0.03395769 0.02702809 0.02753881 0.02431893 0.02048952 0.02168454 0.01828159 0.01628962 0.01425919 0.01099323 0.01206311 0.01108606 0.01057266 0.00955313 0.00792999 0.00849898 0.00594048 0.00551533 0.00604610 0.00409060 0.00318957 0.00301328 0.00254245 0.00210972 0.00282560 0.00312764 0.00244454 0.00174093 0.00179464 0.00167223 0.00100156 0.00033252 0.00099119 0.00043649 0.00026560 0.00086740 0.00044565 0.00022010 0.00056197 0.00000000 0.00032505 0.00012018 0.00032505 0.00023675 0.00043733
2003 0 0 0 0 0 0 0 0 0 0 0.000055 0.000110 0.000110 0.000325 0.000164 0.060300 0.068860 0.062015 0.061210 0.057213 0.057375 0.058815 0.062535 0.059459 0.054489 0.050162 0.045796 0.035964 0.033904 0.029643 0.024112 0.022813 0.019820 0.018117 0.014671 0.014222 0.012202 0.010070 0.009346 0.007788 0.007632 0.007741 0.006184 0.005070 0.004452 0.004551 0.002209 0.002025 0.001902 0.001313 0.000821 0.001287 0.000151 0.000615 0.000494 0.000189 0.000294 0.000288 0.000160 0.000081 0.000241 0.000164 0.000110 0.000079 0.000000 0.000000 0.000134 0.000078 0.000000 0.000071
2004 0  0 0 0 0 0 0 0 0 0 0.0000000 0.0000724 0.0000717 0.0004343 0.0003339 0.0610284 0.0678597 0.0803342 0.0789218 0.0811292 0.0797909 0.0760142 0.0700130 0.0599947 0.0489483 0.0409803 0.0366287 0.0282314 0.0261240 0.0212342 0.0188549 0.0159265 0.0142894 0.0126844 0.0112441 0.0077658 0.0084622 0.0071714 0.0057700 0.0047599 0.0045145 0.0035541 0.0033548 0.0032423 0.0027178 0.0027171 0.0021065 0.0018977 0.0019646 0.0015272 0.0014822 0.0011093 0.0010355 0.0006975 0.0009120 0.0002007 0.0004946 0.0006993 0.0001330 0.0001409 0.0001443 0.0000000 0.0000000 0.0000000 0.0001379 0.0000689 0.0000731 0.0000000 0.0000000 0.0000000
end_table
u_max 0.999
initial_mortality 0
)";
/**
 *
 */
TEST_F(InternalEmptyModel, Processes_Tag_By_Length_simple) {
  AddConfigurationLine(test_model_block_process_tag_by_length, __FILE__, 2);
  AddConfigurationLine(test_remaining_model_process_tag_by_length, __FILE__, 36);
  AddConfigurationLine(simple_tag, __FILE__, 39);
  LoadConfiguration();
  model_->Start(RunMode::kBasic);

  // Expected numbers at age at the end of model run.
  vector<Double> untagged_expected
      = {867085.732971, 753807.994709, 655277.229156, 569237.308067, 494049.061594, 428626.174948, 372014.938201, 323184.560923, 280977.392035, 244376.735365,
         212570.785159, 184904.138748, 160829.930046, 139880.873234, 121652.711560, 105793.838396, 91997.852883,  79997.632633,  69560.332829,  463061.395153};
  vector<Double> tagged_expected = {0.000000,    0.128047,   52.070272,  478.615289, 1238.168213, 1955.857172, 2315.097438, 2242.338317, 1935.162839, 1575.624113,
                                    1249.924069, 982.655718, 772.285575, 609.343775, 483.615589,  386.383453,  310.797418,  251.652705,  205.044264,  1193.702034};

  partition::Category& tagged   = model_->partition().category("tagged");
  partition::Category& untagged = model_->partition().category("untagged");

  for (unsigned i = 0; i < tagged_expected.size(); ++i) {
    EXPECT_NEAR(tagged_expected[i], tagged.data_[i], 0.0001) << " with i = " << i;
    EXPECT_NEAR(untagged_expected[i], untagged.data_[i], 0.0001) << " with i = " << i;
  }
}

/*
 * Test catch when extra length bin is expected
 */
const std::string test_model_block_plus_group =
    R"(
@model
start_year 2001
final_year 2005
min_age 1
max_age 20
age_plus t
length_bins 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76  77  78  79  80
length_plus true
initialisation_phases Equilibrium_state
time_steps step1 step2
)";

TEST_F(InternalEmptyModel, Processes_Tag_By_Length_validate_length_inputs) {
  AddConfigurationLine(test_model_block_plus_group, __FILE__, 2);
  AddConfigurationLine(test_remaining_model_process_tag_by_length, __FILE__, 36);
  AddConfigurationLine(simple_tag, __FILE__, 39);
  LoadConfiguration();
  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);
}

} /* namespace age */
} /* namespace processes */
} /* namespace niwa */

#endif /* TESTMODE */
