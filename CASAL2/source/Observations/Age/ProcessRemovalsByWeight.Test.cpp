/**
 * @file ProcessRemovalsByWeight.Test.cpp
 * @author Ian Doonan (ian.doonan@niwa.co.nz)
 * @date 14/06/2021
 * @section LICENSE
 *
 * Copyright NIWA Science 2021 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "ProcessRemovalsByWeight.h"

#include <iostream>

#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Observations/Manager.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace age {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;

// comments omitted from model
// # length at age
// # F type dynamics so catches could be wrong
// #@selectivity potRet_female
// #type logistic
// #a50   4
// #ato95  1
// #length_based False


const std::string test_cases_observation_process_removals_by_weight =
R"(
@model
start_year 2001
final_year 2005
min_age 1
max_age 80
age_plus false
base_weight_units tonnes
initialisation_phases Define_manually
time_steps 1
length_bins 15  18  21  24  27  30  33  36  39  42  45  48  51  54  57  60  63  66  69  72  75  78  81  84  87  90  93  96  99 102 105

@categories
format sex
names       male    female
age_lengths vb_male vb_female

@initialisation_phase Define_manually
type state_category_by_age
categories male female
min_age 1
max_age 80
table n
male   5000 4615.582 4260.719 3933.139 3630.745 3351.6 3093.917 2855.937 2385.479 1992.538 1664.369 1390.312 1161.455 970.3478 810.7656 677.5068 566.2265 473.2956 395.6836 330.8603 276.713 231.4784 193.6843 162.1023 135.7069 113.6422 95.19409 79.76633 66.86143 56.06414 47.02784 39.46316 33.12855 27.82233 23.37606 19.6491 16.52393 13.90241 11.7025 9.85565 8.304543 7.001253 5.905697 4.984337 4.209107 3.55651 3.006872 2.543711 2.153214 1.823804 1.545772 1.310972 1.112568 0.9448203 0.8029072 0.6827773 0.5810241 0.4947825 0.4216413 0.3595706 0.3068602 0.2620689 0.2239814 0.1915723 0.1639761 0.1404618 0.1204115 0.1033029 0.08869376 0.07621007 0.06553482 0.05639933 0.04857575 0.04187071 0.03611998 0.03118404 0.02694423 0.02329962 0.02016425 0.01746491
female 5000 4615.582 4260.719 3933.139 3630.745 3351.6 3093.917 2855.937 2385.479 1992.538 1664.369 1390.312 1161.455 970.3478 810.7656 677.5068 566.2265 473.2956 395.6836 330.8603 276.713 231.4784 193.6843 162.1023 135.7069 113.6422 95.19409 79.76633 66.86143 56.06414 47.02784 39.46316 33.12855 27.82233 23.37606 19.6491 16.52393 13.90241 11.7025 9.85565 8.304543 7.001253 5.905697 4.984337 4.209107 3.55651 3.006872 2.543711 2.153214 1.823804 1.545772 1.310972 1.112568 0.9448203 0.8029072 0.6827773 0.5810241 0.4947825 0.4216413 0.3595706 0.3068602 0.2620689 0.2239814 0.1915723 0.1639761 0.1404618 0.1204115 0.1033029 0.08869376 0.07621007 0.06553482 0.05639933 0.04857575 0.04187071 0.03611998 0.03118404 0.02694423 0.02329962 0.02016425 0.01746491
end_table

@time_step 1
processes mort

@process mort
type mortality_instantaneous
m 0.08
time_step_ratio 1
relative_m_by_age One
categories *
table catches
year  FishingPot
2001  468.89
2002  0
2003  0
2004  0
2005  0
end_table
table method
method     category    selectivity          u_max time_step penalty
FishingPot format=male    potRet_sel          0.7   1   catchMustBeTaken1
FishingPot format=female  potRet_sel          0.7   1   catchMustBeTaken1
end_table

@selectivity One
type constant
c 1

@selectivity potRet_sel
type double_normal
mu 7.96874
sigma_l 0.28739
sigma_r 88.0876
length_based False

@penalty catchMustBeTaken1
type process
log_scale True
multiplier 1000


@age_length vb_male
type data
length_weight Length_Weight
distribution normal
cv_first 0.07
internal_gaps mean
time_step_measurements_were_made 1
table data
year    1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63  64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79  80
2001    13.1    21.8    30.5    39.3    48  49.2    50.6    51.9    53.1    54.3    55.4    56.5    57.5    58.5    59.5    60.3    61.2    62  62.8    63.6    64.3    64.9    65.6    66.2    66.8    67.4    67.9    68.4    68.9    69.4    69.8    70.3    70.7    71.1    71.4    71.8    72.1    72.5    72.8    73.1    73.4    73.6    73.9    74.1    74.4    74.6    74.8    75  75.2    75.4    75.6    75.7    75.9    76.1    76.2    76.3    76.5    76.6    76.7    76.9    77  77.1    77.2    77.3    77.4    77.5    77.5    77.6    77.7    77.8    77.8    77.9    78  78  78.1    78.2    78.2    78.3    78.3    78.4
end_table

@age_length vb_female
type data
length_weight Length_Weight
distribution normal
cv_first 0.07
internal_gaps mean
time_step_measurements_were_made 1
table data
year    1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63  64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79  80
2001    13.4    22.3    31.2    40.1    49  49  50.4    51.7    53  54.3    55.6    56.9    58.1    59.3    60.5    61.6    62.7    63.8    64.9    66  67  68.1    69.1    70.1    71  72  72.9    73.8    74.7    75.6    76.5    77.3    78.1    79  79.7    80.5    81.3    82  82.8    83.5    84.2    84.9    85.6    86.3    86.9    87.6    88.2    88.8    89.4    90  90.6    91.2    91.7    92.3    92.8    93.3    93.9    94.4    94.9    95.4    95.8    96.3    96.8    97.2    97.7    98.1    98.5    99  99.4    99.8    100.2   100.6   100.9   101.3   101.7   102 102.4   102.7   103.1   103.4
end_table

@length_weight Length_Weight
type basic
units tonnes
a  9.63e-06
b  3.173


@report population_sizes
type partition
time_step 1
years 2001:2005

@observation commWF
type process_removals_by_weight
method_of_removal FishingPot
time_step 1
mortality_instantaneous_process mort
likelihood multinomial
delta 1e-11
years 2001
categories male+female
units kgs

length_weight_cv 0.1
length_weight_distribution normal
length_bins 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 100 101 102 103 104 105
length_bins_n 1*91

weight_bins 0.6 0.7 0.8 0.9 1   1.1 1.2 1.3 1.4 1.5 1.6 1.7 1.8 1.9 2   2.1 2.2 2.3 2.4 2.5 2.6 2.7 2.8 2.9 3   3.1 3.2 3.3 3.4 3.5 3.6 3.7 3.8 3.9 4   4.1 4.2 4.3 4.4 4.5 4.6 4.7 4.8 4.9 5   5.1 5.2 5.3 5.4 5.5 5.6 5.7 5.8 5.9 6   6.1 6.2 6.3 6.4 6.5 6.6 6.7 6.8 6.9 7   7.1 7.2 7.3 7.4 7.5 7.6 7.7 7.8 7.9 8   8.1 8.2 8.3 8.4 8.5 8.6 8.7 8.8 8.9 9   9.1 9.2 9.3 9.4 9.5 9.6 9.7 9.8 9.9 10  10.1    10.2    10.3    10.4    10.5    10.6    10.7    10.8    10.9    11  11.1    11.2    11.3    11.4    11.5    11.6    11.7    11.8    11.9    12  12.1    12.2    12.3    12.4    12.5    12.6    12.7    12.8    12.9    13  13.1    13.2    13.3    13.4    13.5    13.6    13.7    13.8    13.9    14  14.1    14.2    14.3    14.4    14.5    14.6    14.7    14.8    14.9    15  15.1    15.2    15.3    15.4    15.5    15.6    15.7    15.8    15.9    16  16.1    16.2    16.3    16.4    16.5    16.6    16.7    16.8    16.9    17  17.1    17.2    17.3    17.4    17.5    17.6    17.7    17.8    17.9    18  18.1    18.2    18.3    18.4    18.5    18.6    18.7    18.8    18.9    19  19.1    19.2    19.3    19.4    19.5    19.6    19.7    19.8    19.9

table obs
2001 6.66754e-07 2.500328e-06 1.183489e-05 3.800498e-05 0.0001121814 0.0002830371 0.000607913 0.00119949 0.002133613 0.003541964 0.005351701 0.007742848 0.01056155 0.01368629 0.01704257 0.02048952 0.02398881 0.02706971 0.02986625 0.03235707 0.03408446 0.03542297 0.03623408 0.0363931 0.03621224 0.03559266 0.03481873 0.03380309 0.03236441 0.03084021 0.02937952 0.02783815 0.02613259 0.02455872 0.02314903 0.02167184 0.02006096 0.01894865 0.01759364 0.01641732 0.01530217 0.01426654 0.0132419 0.01244796 0.01152918 0.01074224 0.01002081 0.009328389 0.008718809 0.00809156 0.007591494 0.006976247 0.00656686 0.006138471 0.005701247 0.005309196 0.004891808 0.004657277 0.004363738 0.004064366 0.003791497 0.003553299 0.003266761 0.003083571 0.002919716 0.002686852 0.0025445 0.002378812 0.002232959 0.002090607 0.001984927 0.001836907 0.001737061 0.001644549 0.001533868 0.001443689 0.001321173 0.00124383 0.001156485 0.001100644 0.001048137 0.0009544584 0.0009189537 0.0008736145 0.000806439 0.0007726012 0.0007239282 0.0006799224 0.0006454179 0.0005905774 0.0005565729 0.0005339033 0.0004860637 0.0004755623 0.0004485588 0.0004212219 0.0003925514 0.0003768827 0.0003445451 0.0003530462 0.0003180417 0.0003060401 0.000285204 0.0002590339 0.0002428651 0.0002383646 0.0002223625 0.0002155282 0.0002068604 0.0001970258 0.0001918585 0.0001780233 0.0001718558 0.0001643549 0.0001500196 0.0001466859 0.0001286835 0.0001365179 0.0001243496 0.0001286835 0.0001153485 0.000107014 0.0001025134 0.0001056805 8.68447e-05 9.534581e-05 9.051187e-05 8.401101e-05 8.034386e-05 7.517652e-05 6.80089e-05 6.650871e-05 6.834229e-05 5.917442e-05 6.450846e-05 5.36737e-05 5.300695e-05 4.717284e-05 4.367238e-05 4.550596e-05 4.267226e-05 4.400577e-05 3.917179e-05 3.883842e-05 3.983855e-05 3.650478e-05 3.63381e-05 3.400445e-05 2.850374e-05 3.167081e-05 2.867042e-05 2.550334e-05 2.550334e-05 2.700354e-05 2.283632e-05 2.283632e-05 2.283632e-05 1.933586e-05 1.833573e-05 1.916918e-05 2.066937e-05 1.783568e-05 2.016931e-05 1.633547e-05 1.616878e-05 1.433521e-05 1.283502e-05 1.45019e-05 1.583541e-05 1.400183e-05 1.30017e-05 1.233495e-05 1.16682e-05 9.834618e-06 1.133482e-05 1.116813e-05 9.167871e-06 8.167739e-06 9.167868e-06 9.501242e-06 1.050138e-05 7.667674e-06 6.334162e-06 8.334425e-06 7.334293e-06 8.001049e-06 5.667409e-06 5.167342e-06 7.500983e-06 5.50072e-06 6.66754e-06 6.167477e-06 5.50072e-06 4.500591e-06
end_table

table error_values
2001 100
end_table

@ageing_error no_error
type none

)";

/**
 *
 */
TEST_F(InternalEmptyModel, Observation_Process_Removals_By_Weight) {
  AddConfigurationLine(test_cases_observation_process_removals_by_weight, __FILE__, 31); //what is _FILE_ ?
  LoadConfiguration();

  model_->Start(RunMode::kBasic); // kEstimation instead of kBasic

  ObjectiveFunction& obj_function = model_->objective_function();
  // EXPECT_NEAR(-364.2906, obj_function.score(), 1e-3);  //constant part not included<<<<<<<<<<<<
  // obj_function.score() evaluates to 2276.3627871160229
  EXPECT_NEAR(2276.362787, obj_function.score(), 1e-3);

  Observation* observation = model_->managers().observation()->GetObservation("commWF");

  map<unsigned, vector<obs::Comparison> >& comparisons = observation->comparisons();
  ASSERT_EQ(1u, comparisons.size());

  unsigned year = 2001;
  ASSERT_FALSE(comparisons.find(year) == comparisons.end());
  ASSERT_EQ(194u, comparisons[year].size()); // number of weight bins

  //wgt bin 8th = index is 7 =1.3 kg
  EXPECT_EQ("male+female", comparisons[year][7].category_);
  EXPECT_EQ(1.3, comparisons[year][7].length_);		// Is weight_ right? = lower bin weight
  EXPECT_NEAR(0.00119949, comparisons[year][7].observed_, 1e-6);
  EXPECT_NEAR(0.00119949, comparisons[year][7].expected_, 1e-4);	// simulated value so has some error
  //EXPECT_NEAR(88.3886, comparisons[year][7].score_, 1e-4);

  // wgt bin 34th = index 33 = 3.9 kg
  EXPECT_EQ("male+female", comparisons[year][33].category_);
  EXPECT_EQ(3.9, comparisons[year][33].length_);
  EXPECT_NEAR(2.455872e-02 , comparisons[year][33].observed_, 1e-6);
  EXPECT_NEAR(2.455872e-02 , comparisons[year][33].expected_, 1e-4);
 // EXPECT_NEAR(294.428, comparisons[year][33].score_, 1e-3);

  // wgt bin 43th = index 42 = 4.8 kg
  EXPECT_EQ("male+female", comparisons[year][42].category_);
  EXPECT_EQ(4.8, comparisons[year][42].length_);
  EXPECT_NEAR(1.324190e-02, comparisons[year][42].observed_, 1e-6);
  EXPECT_NEAR(1.324190e-02, comparisons[year][42].expected_, 1e-4);
 // EXPECT_NEAR(283.089, comparisons[year][42].score_, 1e-3);

}

} /* namespace processes */
} /* namespace niwa */


#endif /* TESTMODE */
