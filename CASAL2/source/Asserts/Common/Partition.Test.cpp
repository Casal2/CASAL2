/**
 * @file Partition.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 10/10/2018
 * @section LICENSE
 *
 * Copyright NIWA Science ©2018 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// headers
#include "../../Model/Models/Age.h"
#include "../../TestResources/TestFixtures/InternalEmptyModel.h"
#include "Partition.h"

// namespaces
namespace niwa {
namespace asserts {

using niwa::testfixtures::InternalEmptyModel;

const std::string simple_model =
    R"(
@model
start_year 1994
final_year 2008
min_age 1
max_age 50
age_plus t
initialisation_phases iphase1 iphase2
time_steps step_one step_two

@categories
format stage.sex
names immature.male mature.male immature.female mature.female

@initialisation_phase iphase1
years 200
exclude_processes Fishing

@initialisation_phase iphase2
years 1
exclude_processes Fishing

@time_step step_one
processes Recruitment maturation halfM Fishing halfM

@time_step step_two
processes my_ageing

@ageing my_ageing
categories *

@Recruitment Recruitment
type constant
categories stage=immature
proportions 0.5 0.5
R0 997386
age 1

@mortality halfM
type constant_rate
categories *
M 0.065 0.065 0.065 0.065
relative_m_by_age One One One One

@mortality Fishing
type event
categories *
years           1998         1999         2000         2001         2002         2003         2004          2005          2006          2007
catches  1849.153714 14442.000000 28323.203463 24207.464203 47279.000000 58350.943094 82875.872790 115974.547730 113852.472257 119739.517172
U_max 0.99
selectivities FishingSel FishingSel FishingSel FishingSel
penalty event_mortality_penalty

@process maturation
type transition_category
from stage=immature
to stage=mature
proportions 1.0 1.0
selectivities Maturation Maturation

@selectivity One
type constant
c 1

@selectivity Maturation
type logistic_producing
L 5
H 30
a50 8
ato95 3

@selectivity FishingSel
type logistic
a50 8
ato95 3

@catchability CPUEq
type free
q 0.000153139

@derived_quantity abundance
type abundance
categories *
selectivities Maturation Maturation Maturation Maturation
time_step step_one

@estimate
parameter catchability[CPUEq].q
lower_bound 1e-10
upper_bound 1e-1
type uniform

@estimate
parameter process[Recruitment].R0
lower_bound 1e5
upper_bound 1e10
type uniform_log

@estimate
parameter selectivity[FishingSel].a50
lower_bound 1
upper_bound 20
type uniform

@estimate
parameter selectivity[FishingSel].ato95
lower_bound 0.01
upper_bound 50
type uniform

@penalty event_mortality_penalty
type process
log_scale False
multiplier 10

@assert immature.male
type partition
values 0.000000 437900.044732 384500.085693 337586.935828 296338.439446 246996.897569 199758.551826 144746.120531 86358.900382 40484.847754 15594.173933 5380.421692 1772.213031 575.327688 186.447288 60.564281 19.734772 6.449706 2.113516 0.693984 0.228147 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert mature.male
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 12999.836714 28054.528897 54244.502735 86358.900382 108029.834496 111036.247064 102228.012149 89850.547300 77834.345299 67307.470836 58341.148080 50727.296267 44238.533929 38682.773233 33893.2181021 29732.400042 26097.552416 22912.552894 20118.210819 17665.312973 15511.699919 13620.709794 11960.268873 10502.252810 9221.978693 8097.776861 7110.620709 6243.803503 5482.655315 4814.294580 4227.410074 3712.069471 3259.551241 2862.197052 2513.282154 2206.901576 1937.870190 1701.634960 1494.197883 1312.048334 1152.103647 1011.656949 888.331344 780.039695 5618.733544

@assert immature.female
type partition
values 0.000000 437900.044732 384500.085693 337586.935828 296338.439446 246996.897569 199758.551826 144746.120531 86358.900382 40484.847754 15594.173933 5380.421692 1772.213031 575.327688 186.447288 60.564281 19.734772 6.449706 2.113516 0.693984 0.228147 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert mature.female
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 12999.836714 28054.528897 54244.502735 86358.900382 108029.834496 111036.247064 102228.012149 89850.547300 77834.345299 67307.470836 58341.148080 50727.296267 44238.533929 38682.773233 33893.2181021 29732.400042 26097.552416 22912.552894 20118.210819 17665.312973 15511.699919 13620.709794 11960.268873 10502.252810 9221.978693 8097.776861 7110.620709 6243.803503 5482.655315 4814.294580 4227.410074 3712.069471 3259.551241 2862.197052 2513.282154 2206.901576 1937.870190 1701.634960 1494.197883 1312.048334 1152.103647 1011.656949 888.331344 780.039695 5618.733544
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Asserts_Partition) {
  AddConfigurationLine(simple_model, __FILE__, 26);
  LoadConfiguration();
  EXPECT_NO_THROW(model_->Start(RunMode::kBasic));
}

/**
 *
 */
const std::string simple_model_fail =
    R"(
@model
start_year 1994
final_year 2008
min_age 1
max_age 50
age_plus t
initialisation_phases iphase1 iphase2
time_steps step_one step_two

@categories
format stage.sex
names immature.male mature.male immature.female mature.female

@initialisation_phase iphase1
years 200
exclude_processes Fishing

@initialisation_phase iphase2
years 1
exclude_processes Fishing

@time_step step_one
processes Recruitment maturation halfM Fishing halfM

@time_step step_two
processes my_ageing

@ageing my_ageing
categories *

@Recruitment Recruitment
type constant
categories stage=immature
proportions 0.5 0.5
R0 997386
age 1

@mortality halfM
type constant_rate
categories *
M 0.065 0.065 0.065 0.065
relative_m_by_age One One One One

@mortality Fishing
type event
categories *
years           1998         1999         2000         2001         2002         2003         2004          2005          2006          2007
catches  1849.153714 14442.000000 28323.203463 24207.464203 47279.000000 58350.943094 82875.872790 115974.547730 113852.472257 119739.517172
U_max 0.99
selectivities FishingSel FishingSel FishingSel FishingSel
penalty event_mortality_penalty

@process maturation
type transition_category
from stage=immature
to stage=mature
proportions 1.0 1.0
selectivities Maturation Maturation

@selectivity One
type constant
c 1

@selectivity Maturation
type logistic_producing
L 5
H 30
a50 8
ato95 3

@selectivity FishingSel
type logistic
a50 8
ato95 3

@catchability CPUEq
type free
q 0.000153139

@derived_quantity abundance
type abundance
categories *
selectivities Maturation Maturation Maturation Maturation
time_step step_one

@estimate
parameter catchability[CPUEq].q
lower_bound 1e-10
upper_bound 1e-1
type uniform

@estimate
parameter process[Recruitment].R0
lower_bound 1e5
upper_bound 1e10
type uniform_log

@estimate
parameter selectivity[FishingSel].a50
lower_bound 1
upper_bound 20
type uniform

@estimate
parameter selectivity[FishingSel].ato95
lower_bound 0.01
upper_bound 50
type uniform

@penalty event_mortality_penalty
type process
log_scale False
multiplier 10

@assert immature.male
type partition
values 0.000000 437900.044732 384500.085793 337586.935828 296338.439446 246995.897569 199758.551826 144746.120531 86358.900382 40484.847754 15594.173933 5380.421692 1772.213031 575.327688 186.447288 60.564281 19.734772 6.449706 2.113516 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert mature.male
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 12999.836714 28054.528887 54244.502735 86358.900382 108029.834496 111036.247064 100228.012149 89850.547300 77834.345299 67307.470836 58341.148080 50727.296267 44238.533929 38682.773233 33893.912086 29732.628190 26097.552416 22912.552894 20118.210819 17665.312973 15511.699919 13620.709794 11960.268873 10502.252810 9221.978693 8097.776861 7110.620709 6243.803503 5482.655315 4814.294580 4227.410074 3712.069471 3259.551241 2862.197052 2513.282154 2206.901576 1937.870190 1701.634960 1494.197883 1312.048334 1152.103647 1011.656949 888.331344 780.039695 5618.733544

@assert immature.female
type partition
values 0.000000 437900.045732 384500.085693 337586.935128 296338.439446 246996.897569 199758.551826 144746.120531 86358.900382 40484.847754 15594.173933 5380.421692 1572.213031 575.327688 186.447288 60.564281 19.734772 6.449706 2.113516 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000

@assert mature.female
type partition
values 0.000000 0.000000 0.000000 0.000000 0.000000 12999.836714 28054.528897 54244.502735 86358.900382 107029.834496 111036.247064 102228.012149 89850.547300 77834.345299 67307.470836 58341.148080 50727.296267 44238.533929 38682.773233 33893.912086 29732.628190 26097.552416 22912.552894 20118.210819 17665.312973 15511.699919 13620.709794 11960.268873 10502.252810 9221.978693 8097.776861 7110.620709 6243.803503 5482.655315 4814.294580 4227.410074 3712.069471 3259.551241 2862.197052 2513.282154 2206.901576 1937.870190 1701.634960 1494.197883 1312.048334 1152.103647 1011.656949 888.331344 780.039695 5618.733544
)";

/**
 *
 */
TEST_F(InternalEmptyModel, Asserts_Partition_Throws_Exception) {
  AddConfigurationLine(simple_model_fail, __FILE__, 173);
  LoadConfiguration();

  EXPECT_THROW(model_->Start(RunMode::kBasic), std::string);
}

}  // namespace asserts
}  // namespace niwa

#endif
