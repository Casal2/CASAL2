/**
 * @file TagByAge.Test.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 10/02/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file contains unit tests for the TagByAge process
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "Common/ObjectiveFunction/ObjectiveFunction.h"
#include "Common/Processes/Manager.h"
#include "Common/Model/Model.h"
#include "Common/Partition/Partition.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace age {
namespace processes {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

const std::string test_cases_process_tag_by_age =
R"(
@model
start_year 1994
final_year 2008
min_age 1
max_age 12
age_plus t
base_weight_units kgs
initialisation_phases iphase1 iphase2
time_steps step_one=[processes=Recruitment] step_two=[processes=Tagging] step_three=[processes=Ageing]

@categories
format stage.sex
names immature.male mature.male immature.female mature.female

@initialisation_phase iphase1
years 200

@initialisation_phase iphase2
years 1

@ageing Ageing
categories *

@Recruitment Recruitment
type constant
categories immature.male immature.female
proportions 0.5 0.5
R0 997386
age 1

@tag Tagging
type by_age
years 2008
from immature.male immature.female
to mature.male mature.female
selectivities [type=constant; c=0.25] [type=constant; c=0.4]
min_age 3
max_age 6
loss_rate 0
loss_rate_selectivities loss_rate1=[type=constant; c=1] lossrate2=[type=constant; c=1]
penalty [type=process]
table numbers
year 3 4 5 6
2008 1000 2000 3000 4000
end_table

@report DQ
type derived_quantity
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_Tag_By_Age) {
  AddConfigurationLine(test_cases_process_tag_by_age, __FILE__, 36);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  // 0.000000 0.000000 0.000000 384.615385 769.230769 1153.846154 1538.461538 0.000000 0.000000 0.000000 0.000000 0.000000
  partition::Category& male   = model_->partition().category("mature.male");
  EXPECT_DOUBLE_EQ(0.0,         male.data_[0]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[1]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[2]);
  EXPECT_DOUBLE_EQ(384.61538461538458,  male.data_[3]);
  EXPECT_DOUBLE_EQ(769.23076923076917,  male.data_[4]);
  EXPECT_DOUBLE_EQ(1153.84615384615384, male.data_[5]);
  EXPECT_DOUBLE_EQ(1538.4615384615383, male.data_[6]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[7]);

  // 615.384615 1230.769231 1846.153846 2461.538462 0.000000 0.000000 0.000000 0.000000 0.000000
  partition::Category& female = model_->partition().category("mature.female");
  EXPECT_DOUBLE_EQ(0.0,         female.data_[0]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[1]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[2]);
  EXPECT_DOUBLE_EQ(615.38461538461547,  female.data_[3]);
  EXPECT_DOUBLE_EQ(1230.7692307692309,  female.data_[4]);
  EXPECT_DOUBLE_EQ(1846.1538461538462,   female.data_[5]);
  EXPECT_DOUBLE_EQ(2461.5384615384619,  female.data_[6]);
  EXPECT_DOUBLE_EQ(0.0,                 female.data_[7]);
}

/**
 * Loss rate
 */
const std::string test_cases_process_tag_by_age_with_loss_rate =
R"(
@model
start_year 1994
final_year 2008
min_age 1
max_age 12
age_plus t
base_weight_units kgs
initialisation_phases iphase1 iphase2
time_steps step_one=[processes=Recruitment] step_two=[processes=Tagging] step_three=[processes=Ageing]

@categories
format stage.sex
names immature.male mature.male immature.female mature.female

@initialisation_phase iphase1
years 200

@initialisation_phase iphase2
years 1

@ageing Ageing
categories *

@Recruitment Recruitment
type constant
categories immature.male immature.female
proportions 0.5 0.5
R0 997386
age 1

@tag Tagging
type by_age
years 2008
from immature.male immature.female
to mature.male mature.female
selectivities [type=constant; c=0.25] [type=constant; c=0.4]
min_age 3
max_age 6
loss_rate 0.02
loss_rate_selectivities lr1=[type=constant; c=1] lr2=[type=constant; c=1]
penalty [type=process]
table numbers
year 3 4 5 6
2008 500 750 1000 2000
end_table

@report DQ
type derived_quantity
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_Tag_By_Age_With_Loss_Rate) {
  AddConfigurationLine(test_cases_process_tag_by_age_with_loss_rate, __FILE__, 119);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  partition::Category& male   = model_->partition().category("mature.male");
  EXPECT_DOUBLE_EQ(0.0,         male.data_[0]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[1]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[2]);
  EXPECT_DOUBLE_EQ(192.30769230769229, male.data_[3]);
  EXPECT_DOUBLE_EQ(288.46153846153845, male.data_[4]);
  EXPECT_DOUBLE_EQ(384.61538461538458, male.data_[5]);
  EXPECT_DOUBLE_EQ(769.23076923076917, male.data_[6]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[7]);

  partition::Category& female = model_->partition().category("mature.female");
  EXPECT_DOUBLE_EQ(0.0,         female.data_[0]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[1]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[2]);
  EXPECT_DOUBLE_EQ(307.69230769230774,  female.data_[3]);
  EXPECT_DOUBLE_EQ(461.53846153846155,  female.data_[4]);
  EXPECT_DOUBLE_EQ(615.38461538461547,  female.data_[5]);
  EXPECT_DOUBLE_EQ(1230.7692307692309,  female.data_[6]);
  EXPECT_DOUBLE_EQ(0.0,                 female.data_[7]);
}

/**
 * Loss rate selectivities
 */
const std::string test_cases_process_tag_by_age_with_loss_rate_selectivities =
R"(
@model
start_year 1994
final_year 2010
min_age 1
max_age 12
base_weight_units kgs
age_plus t
initialisation_phases iphase1 iphase2
time_steps step_one=[processes=Recruitment] step_two=[processes=Tagging] step_three=[processes=Ageing]

@categories
format stage.sex
names immature.male mature.male immature.female mature.female

@initialisation_phase iphase1
years 200

@initialisation_phase iphase2
years 1

@ageing Ageing
categories *

@Recruitment Recruitment
type constant
categories immature.male immature.female
proportions 0.5 0.5
R0 997386
age 1

@tag Tagging
type by_age
years 2008
from immature.male immature.female
to mature.male mature.female
selectivities [type=constant; c=0.25] [type=constant; c=0.4]
min_age 3
max_age 6
loss_rate 0.02
loss_rate_selectivities lr1=[type=logistic; a50=11.9; ato95=5.25] lr2=[type=constant; c=0.5]
penalty [type=process]
table numbers
year 3 4 5 6
2008 500 750 1000 2000
end_table

@report DQ
type derived_quantity
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_Tag_By_Age_With_Loss_Rate_Selectivities) {
  AddConfigurationLine(test_cases_process_tag_by_age_with_loss_rate_selectivities, __FILE__, 202);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  /**
   * Note the results have moved compared to the other tests because this
   * one has extra years
   */

  // 0.000000 0.000000 0.000000 0.000000 0.000000 190.389423 285.584135 380.778846 761.557692 0.000000 0.000000 0.000000
  partition::Category& male   = model_->partition().category("mature.male");
  EXPECT_DOUBLE_EQ(0.0,         male.data_[0]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[1]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[2]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[3]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[4]);
  EXPECT_DOUBLE_EQ(192.18385769115466, male.data_[5]);
  EXPECT_DOUBLE_EQ(288.14027764600274, male.data_[6]);
  EXPECT_DOUBLE_EQ(383.88143638402539, male.data_[7]);
  EXPECT_DOUBLE_EQ(766.75435176196413, male.data_[8]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[9]);

  // 0.000000 0.000000 0.000000 0.000000 0.000000 302.788923 454.183385 605.577846 1211.155692 0.000000 0.000000 0.000000
  partition::Category& female = model_->partition().category("mature.female");
  EXPECT_DOUBLE_EQ(0.0,         female.data_[0]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[1]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[2]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[3]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[4]);
  EXPECT_DOUBLE_EQ(301.56923076923078,  female.data_[5]);
  EXPECT_DOUBLE_EQ(452.35384615384612,  female.data_[6]);
  EXPECT_DOUBLE_EQ(603.13846153846157,  female.data_[7]);
  EXPECT_DOUBLE_EQ(1206.2769230769231,  female.data_[8]);
  EXPECT_DOUBLE_EQ(0.0,                 female.data_[9]);
}

/**
 * Selectivities
 */
const std::string test_cases_process_tag_by_age_with_selectivities =
R"(
@model
start_year 1994
final_year 2008
min_age 1
max_age 12
age_plus t
initialisation_phases iphase1 iphase2
time_steps step_one=[processes=Recruitment] step_two=[processes=Tagging] step_three=[processes=Ageing]

@categories
format stage.sex
names immature.male mature.male immature.female mature.female

@initialisation_phase iphase1
years 200

@initialisation_phase iphase2
years 1

@ageing Ageing
categories *

@Recruitment Recruitment
type constant
categories immature.male immature.female
proportions 0.5 0.5
R0 997386
age 1

@tag Tagging
type by_age
years 2008
from immature.male immature.female
to mature.male mature.female
selectivities [type=logistic; a50=9; ato95=3] [type=constant; c=0.7]
min_age 3
max_age 6
loss_rate 0.02
loss_rate_selectivities [type=logistic; a50=11.9; ato95=5.25] [type=constant; c=0.5]
penalty [type=process]
table numbers
year 3 4 5 6
2008 500 750 1000 2000
end_table

@report DQ
type derived_quantity
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_Tag_By_Age_With_Selectivities) {
  AddConfigurationLine(test_cases_process_tag_by_age_with_selectivities, __FILE__, 292);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  // 0.000000 0.000000 0.000000 0.000000 0.000000 190.389423 285.584135 380.778846 761.557692 0.000000 0.000000 0.000000
  partition::Category& male   = model_->partition().category("mature.male");
  EXPECT_DOUBLE_EQ(0.0,         male.data_[0]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[1]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[2]);
  EXPECT_DOUBLE_EQ(13.444567616736462, male.data_[3]);
  EXPECT_DOUBLE_EQ(50.0,               male.data_[4]);
  EXPECT_DOUBLE_EQ(149.60525565892942, male.data_[5]);
  EXPECT_DOUBLE_EQ(560.55678808099083, male.data_[6]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[7]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[8]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[9]);

  // 0.000000 0.000000 0.000000 0.000000 0.000000 302.788923 454.183385 605.577846 1211.155692 0.000000 0.000000 0.000000
  partition::Category& female = model_->partition().category("mature.female");
  EXPECT_DOUBLE_EQ(0.0,         female.data_[0]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[1]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[2]);
  EXPECT_DOUBLE_EQ(486.55543238326356, female.data_[3]);
  EXPECT_DOUBLE_EQ(699.99999999999989, female.data_[4]);
  EXPECT_DOUBLE_EQ(850.39474434107069, female.data_[5]);
  EXPECT_DOUBLE_EQ(1439.4432119190092, female.data_[6]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[7]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[8]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[9]);
}

/**
 * Selectivities
 */
const std::string test_cases_process_tag_by_age_with_proportions_table =
R"(
@model
start_year 1994
final_year 2010
min_age 1
max_age 12
age_plus t
initialisation_phases iphase1 iphase2
time_steps step_one=[processes=Recruitment] step_two=[processes=Tagging] step_three=[processes=Ageing]

@categories
format stage.sex
names immature.male mature.male immature.female mature.female

@initialisation_phase iphase1
years 200

@initialisation_phase iphase2
years 1

@ageing Ageing
categories *

@Recruitment Recruitment
type constant
categories immature.male immature.female
proportions 0.5 0.5
R0 997386
age 1

@tag Tagging
type by_age
years 2008
from immature.male immature.female
to mature.male mature.female
selectivities [type=constant; c=0.25] [type=constant; c=0.4]
min_age 3
max_age 6
loss_rate 0.02
loss_rate_selectivities [type=logistic; a50=11.9; ato95=5.25] [type=constant; c=0.5]
penalty [type=process]
n 10000
table proportions
year 3 4 5 6
2008 0.1 0.2 0.3 0.4
end_table

@report DQ
type derived_quantity
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Processes_Tag_By_Age_With_Proportions_Table) {
  AddConfigurationLine(test_cases_process_tag_by_age_with_proportions_table, __FILE__, 379);
  LoadConfiguration();

  model_->Start(RunMode::kBasic);

  // 0.000000 0.000000 0.000000 0.000000 0.000000 380.778846 761.557692 1142.336538 1523.115385 0.000000 0.000000 0.000000
  partition::Category& male   = model_->partition().category("mature.male");
  EXPECT_DOUBLE_EQ(0.0,         male.data_[0]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[1]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[2]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[3]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[4]);
  EXPECT_DOUBLE_EQ(384.36771538230931, male.data_[5]);
  EXPECT_DOUBLE_EQ(768.37407372267387, male.data_[6]);
  EXPECT_DOUBLE_EQ(1151.6443091520762, male.data_[7]);
  EXPECT_DOUBLE_EQ(1533.5087035239283, male.data_[8]);
  EXPECT_DOUBLE_EQ(0.0,         male.data_[9]);

  // 0.000000 0.000000 0.000000 0.000000 0.000000 605.577846 1211.155692 1816.733538 2422.311385 0.000000 0.000000 0.000000
  partition::Category& female = model_->partition().category("mature.female");
  EXPECT_DOUBLE_EQ(0.0,         female.data_[0]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[1]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[2]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[3]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[4]);
  EXPECT_DOUBLE_EQ(603.13846153846157, female.data_[5]);
  EXPECT_DOUBLE_EQ(1206.2769230769231, female.data_[6]);
  EXPECT_DOUBLE_EQ(1809.4153846153845, female.data_[7]);
  EXPECT_DOUBLE_EQ(2412.5538461538463, female.data_[8]);
  EXPECT_DOUBLE_EQ(0.0,         female.data_[9]);
}

} /* namespace processes */
} /* namespace age */
} /* namespace niwa */


#endif /* TESTMODE */
