/**
 * @file VonBertalanffy.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 19/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "VonBertalanffy.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

#include "TestResources/MockClasses/Managers.h"
#include "TestResources/MockClasses/Model.h"
#include "Partition/Partition.h"
#include "Selectivities/Common/Logistic.h"
#include "TimeSteps/Manager.h"
#include "Utilities/Distribution.h"
// namespaces
namespace niwa {
namespace agelengths {
using ::testing::Return;
using ::testing::ReturnRef;

// classes
class MockTimeStepManager : public timesteps::Manager {
public:
  unsigned time_step_index_ = 0;
  unsigned current_time_step() const override final { return time_step_index_; }
};

class MockVonBertalanffy : public VonBertalanffy {
public:
  MockVonBertalanffy(Distribution distribution = Distribution::kNormal)
  : VonBertalanffy(nullptr) {
    distribution_ = distribution;
  }

  MockVonBertalanffy(Model& model, Double linf, Double k, Double t0, bool by_length,
      Double cv_first, Double cv_last, vector<Double> time_step_proportions, bool casal_switch = false, Distribution distributuion = Distribution::kNormal) : VonBertalanffy(&model) {
    linf_ = linf;
    k_ = k;
    t0_ = t0;
    by_length_ = by_length;
    cv_first_ = cv_first;
    cv_last_ = cv_last;
    time_step_proportions_ = time_step_proportions;
    casal_normal_cdf_ = casal_switch;
    distribution_ = distributuion;
  }

  void MockBuildCV() {
    this->BuildCV();
  }
};


TEST(AgeLengths, VonBertalanffy_MeanLength) {
  MockModel model;
  model.bind_calls();

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, 4, false, 0.2, 0.2, {1.0});
  ASSERT_NO_THROW(von_bertalanffy.MockBuildCV());

  EXPECT_DOUBLE_EQ(0.0, von_bertalanffy.mean_length(0, 1));
  EXPECT_DOUBLE_EQ(0.0, von_bertalanffy.mean_length(0, 2));
  EXPECT_DOUBLE_EQ(0.0, von_bertalanffy.mean_length(0, 3));
  EXPECT_DOUBLE_EQ(4.9596000375416427, von_bertalanffy.mean_length(0, 4));
  EXPECT_DOUBLE_EQ(9.611729668428497,  von_bertalanffy.mean_length(0, 5));
  EXPECT_DOUBLE_EQ(13.975450520665413, von_bertalanffy.mean_length(0, 6));
  EXPECT_DOUBLE_EQ(18.068642496620129, von_bertalanffy.mean_length(0, 7));
  EXPECT_DOUBLE_EQ(21.908077034104725, von_bertalanffy.mean_length(0, 8));
  EXPECT_DOUBLE_EQ(25.509485825636233, von_bertalanffy.mean_length(0, 9));
  EXPECT_DOUBLE_EQ(28.887625277446702, von_bertalanffy.mean_length(0, 10));
}

TEST(AgeLengths, VonBertalanffy_MeanLength_2) {
  MockModel model;
  model.bind_calls();

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, 4, false, 0.2, 0.2, {1.0}, false, Distribution::kLogNormal);
  ASSERT_NO_THROW(von_bertalanffy.MockBuildCV());

  EXPECT_DOUBLE_EQ(0.0, von_bertalanffy.mean_length(0, 1));
  EXPECT_DOUBLE_EQ(0.0, von_bertalanffy.mean_length(0, 2));
  EXPECT_DOUBLE_EQ(0.0, von_bertalanffy.mean_length(0, 3));
  EXPECT_DOUBLE_EQ(4.9596000375416427, von_bertalanffy.mean_length(0, 4));
  EXPECT_DOUBLE_EQ(9.611729668428497,  von_bertalanffy.mean_length(0, 5));
  EXPECT_DOUBLE_EQ(13.975450520665413, von_bertalanffy.mean_length(0, 6));
  EXPECT_DOUBLE_EQ(18.068642496620129, von_bertalanffy.mean_length(0, 7));
  EXPECT_DOUBLE_EQ(21.908077034104725, von_bertalanffy.mean_length(0, 8));
  EXPECT_DOUBLE_EQ(25.509485825636233, von_bertalanffy.mean_length(0, 9));
  EXPECT_DOUBLE_EQ(28.887625277446702, von_bertalanffy.mean_length(0, 10));
}


/**
 *
 */
TEST(AgeLengths, VonBertalanffy_CV) {
  MockModel model;
  model.bind_calls();

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, 4, false, 0.2, 0.2, {1.0});
  ASSERT_NO_THROW(von_bertalanffy.MockBuildCV());

  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 1));
  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 2));
  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 3));
  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 4));
  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 5));
  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 6));
  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 7));
  EXPECT_DOUBLE_EQ(0.2, von_bertalanffy.cv(1990, 0, 8));
}

/**
 *
 */
TEST(AgeLengths, VonBertalanffy_CV_2) {
  MockModel model;
  model.bind_calls();

  MockVonBertalanffy von_bertalanffy(model, 169.07, 0.093, -0.256, true, 0.102, 0.0, {0.0, 0.0}, true);
  ASSERT_NO_THROW(von_bertalanffy.MockBuildCV());

  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 1));
  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 2));
  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 3));
  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 4));
  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 5));
  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 6));
  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 7));
  EXPECT_DOUBLE_EQ(0.10199999999999999, von_bertalanffy.cv(1990, 0, 8));
}

///**
// * Test the cumulative normal function that calculates probability of a being in a length bin at a known age
// * This test is for the normal distribution
// */
//TEST(AgeLengths, VonBertalanffy_CummulativeNormal) {
//  Double mu = 35.49858;
//  Double cv = 0.1;
//  vector<Double> length_bins = {0, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 ,32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
//  vector<Double> vprop_in_length(length_bins.size(), 0.0);
//  bool plus_grp = 1;
//
//  MockVonBertalanffy von_bertalanffy;
//  von_bertalanffy.MockCummulativeNormal(mu, cv, vprop_in_length, length_bins, plus_grp);
//
//  vector<Double> expected = {3.8713535710499514e-009, 1.5960216925847703e-008, 7.422358561104403e-008, 3.1901955588331532e-007, 1.2672619864595447e-006, 4.6525401673491729e-006,
//      1.5786604316003761e-005, 4.9506445653380027e-005,0.00014348551812060073, 0.00038434913282614502,0.00095150900849361175, 0.0021770396325317964, 0.0046034492460040877,
//      0.0089962477651120976, 0.016247952527619458, 0.027120294871666895, 0.041835938013406682, 0.059643893860880981, 0.078586144395677904, 0.095695106434901311,0.10769489117184539,
//      0.11201216057229546,0.10767078378048922,0.095652266453105428,0.078533378916068819, 0.059590504250288112, 0.041789131783027234 , 0.027083887651074501, 0.01622250783484136,
//      0.0089801483958439343, 0.0045941822881506722, 0.002172170763177772, 0.00094916847523229819, 0.00059778133048304927};
//
//  ASSERT_EQ(expected.size(), vprop_in_length.size());
//  for (unsigned i = 0; i < expected.size(); ++i) {
//    EXPECT_DOUBLE_EQ(expected[i], vprop_in_length[i]) << " with i = " << i;
//  }
//}
//
///**
// * Test the Cumulative normal function when the distribution is specified as "lognormal" with no plus group
// */
//TEST(AgeLengths, VonBertalanffy_CummulativeNormal_2) {
//  Double mu = 35.49858;
//  Double cv = 0.1;
//  vector<Double> length_bins = {0, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 ,32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};
//  vector<Double> vprop_in_length(length_bins.size() - 1, 0.0);
//  bool plus_grp = 0;
//
//  MockVonBertalanffy von_bertalanffy(AgeLength::Distribution::kLogNormal);
//  von_bertalanffy.MockCummulativeNormal(mu, cv, vprop_in_length, length_bins, plus_grp);
//
//  vector<Double> expected = {0, 9.9920072216264089e-016,1.1390888232654106e-013, 6.907807659217724e-012, 2.4863089365112501e-010, 5.6808661108576075e-009, 8.7191919018181352e-008,
//      9.4269457673323842e-007, 7.4745056608538363e-006, 4.4982380957292456e-005, 0.00021163731992057677, 0.00079862796125962365, 0.0024715534075264722, 0.0063962867724943751,0.01408161729231916,
//      0.026773528172936767, 0.044555539731829574,  0.065676687795628297, 0.086665248035340148,0.1033532304575121, 0.11234199264093081, 0.11215706824927596, 0.10355520403577334,
//      0.088978990579101747, 0.071552403002032916, 0.054126600808507175, 0.038696543948670059, 0.026257468168220055, 0.016976053665368585, 0.010494572238876954, 0.0062237099943515117,
//      0.0035513001369547048,0.0019551097231892411 };
//
//  ASSERT_EQ(expected.size(), vprop_in_length.size());
//  for (unsigned i = 0; i < expected.size(); ++i) {
//    EXPECT_DOUBLE_EQ(expected[i], vprop_in_length[i]) << " with i = " << i;
//  }
//}
//
//
//TEST(AgeLengths, VonBertalanffy_CummulativeNormal_3) {
//  Double mu = 40.081628;
//  Double cv = 0.1;
//  vector<Double> length_bins = {0, 20, 40, 60 , 80, 110};
//  vector<Double> vprop_in_length(length_bins.size() - 1, 0.0);
//  bool plus_grp = 0;
//
//  MockVonBertalanffy von_bertalanffy;
//  von_bertalanffy.MockCummulativeNormal(mu, cv, vprop_in_length, length_bins, plus_grp);
//
//  vector<Double> expected = {2.7232626398365767e-007, 0.49187561267029634, 0.50812377877585069, 3.3622758899287675e-007,0};
//
//  ASSERT_EQ(expected.size(), vprop_in_length.size());
//  for (unsigned i = 0; i < expected.size(); ++i) {
//    EXPECT_DOUBLE_EQ(expected[i], vprop_in_length[i]) << " with i = " << i;
//  }
//}
//
//
/**
 * Test the DoAgeLengthConversion() so that we know we are applying the right probabilities to the right part of the partition
 */
//TEST(AgeLengths, VonBertalanffy_DoAgeLengthConversion) {
//  MockModel model;
//  MockManagers mock_managers(&model);
//  MockTimeStepManager time_step_manager;
//  time_step_manager.time_step_index_ = 0;
//  vector<string> time_steps = {"0", "1", "2"};
//  vector<unsigned> years = { 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999 };
//  EXPECT_CALL(mock_managers, time_step()).WillRepeatedly(Return(&time_step_manager));
//
//  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(5));
//  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(10));
//  EXPECT_CALL(model, age_spread()).WillRepeatedly(Return(6));
//  EXPECT_CALL(model, start_year()).WillRepeatedly(Return(1990));
//  EXPECT_CALL(model, final_year()).WillRepeatedly(Return(1999));
//  EXPECT_CALL(model, current_year()).WillRepeatedly(Return(1999));
//  EXPECT_CALL(model, age_plus()).WillRepeatedly(Return(true));
//  EXPECT_CALL(model, managers()).WillRepeatedly(ReturnRef(mock_managers));
//  EXPECT_CALL(model, time_steps()).WillRepeatedly(ReturnRef(time_steps));
//  EXPECT_CALL(model, years()).WillRepeatedly(Return(years));
//  EXPECT_CALL(model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));
//
//  selectivities::Logistic logistic(&model);
//  logistic.parameters().Add(PARAM_LABEL, "unit_test_logistic", __FILE__, __LINE__);
//  logistic.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
//  logistic.parameters().Add(PARAM_A50,   "6",  __FILE__, __LINE__);
//  logistic.parameters().Add(PARAM_ATO95, "3",  __FILE__, __LINE__);
//  logistic.Validate();
//  logistic.Build();
//
//  model.partition().BuildMeanLengthData();
//
//  partition::Category male(&model);
//  male.min_age_ = 5;
//  male.max_age_ = 10;
//  male.data_.assign((male.max_age_ - male.min_age_) + 1, 100); // add 100 fish to each age class
//
//  male.mean_length_by_time_step_age_[0][0][0] = 21.84162;
//  male.mean_length_by_time_step_age_[0][0][1] = 23.45148;
//  male.mean_length_by_time_step_age_[0][0][2] = 25.00753;
//  male.mean_length_by_time_step_age_[0][0][3] = 26.51156;
//  male.mean_length_by_time_step_age_[0][0][4] = 27.96531;
//  male.mean_length_by_time_step_age_[0][0][5] = 29.37047;
//
//  MockVonBertalanffy von_bertalanffy(model, 70, 0.034, -6, false, 0.1, 0.1, {0.0});
//
//  ASSERT_NO_THROW(von_bertalanffy.MockBuildCV());
//
//  EXPECT_DOUBLE_EQ(0.1, von_bertalanffy.cv(1999, 0, 5));
//  EXPECT_DOUBLE_EQ(0.1, von_bertalanffy.cv(1999, 0, 10));
//
////  von_bertalanffy.
////  von_bertalanffy.DoAgeToLengthMatrixConversion(&male, {0, 10, 20, 25, 30}, false, &logistic);
//
//  // Check that the CV is being built appropriately and that the mean is stored correctly
//
//
//  EXPECT_DOUBLE_EQ(21.84162, male.mean_length_by_time_step_age_[0][0][0]);
//  EXPECT_DOUBLE_EQ(29.37047, male.mean_length_by_time_step_age_[0][0][5]);
//
//  //Run through length for the min and max age
//  vector<Double> expec1 = {8.0702029084675753e-007, 5.4401509671141266, 19.80017616005102, 2.016943794826572};
//  vector<Double> expec2 = {6.3289251617248965e-009, 0.20879661176386954, 13.517948004834885, 59.096798632366863};
//
//  // min age age =5
//  unsigned age_index = 0;
//  for(unsigned bin = 0; bin < 4; ++bin)
//  EXPECT_DOUBLE_EQ(expec1[bin], male.age_length_matrix_[age_index][bin]) << " where age = " << (male.min_age_ + age_index) << " where class_bin = " << bin;
//  // age = 9
//  age_index = 4;
//  for(unsigned bin = 0; bin < 4; ++bin)
//  EXPECT_DOUBLE_EQ(expec2[bin], male.age_length_matrix_[age_index][bin]) << " where age = " << (male.min_age_ + age_index) << " where class_bin = " << bin;
//}
//
///*
//* Test if there is a plus group with different von bert parameters
//*
//*/
//
//TEST(AgeLengths, VonBertalanffy_DoAgeLengthConversion_plusGrp) {
//  MockModel model;
//  MockManagers mock_managers(&model);
//
//  MockTimeStepManager time_step_manager;
//  time_step_manager.time_step_index_ = 0;
//  vector<string> time_steps = {"0", "1", "2"};
//  vector<unsigned> years = { 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999 };
//
//  EXPECT_CALL(mock_managers, time_step()).WillRepeatedly(Return(&time_step_manager));
//
//  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(5));
//  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(10));
//  EXPECT_CALL(model, age_spread()).WillRepeatedly(Return(6));
//  EXPECT_CALL(model, start_year()).WillRepeatedly(Return(1999));
//  EXPECT_CALL(model, final_year()).WillRepeatedly(Return(1999));
//  EXPECT_CALL(model, current_year()).WillRepeatedly(Return(1999));
//  EXPECT_CALL(model, age_plus()).WillRepeatedly(Return(true));
//  EXPECT_CALL(model, managers()).WillRepeatedly(ReturnRef(mock_managers));
//  EXPECT_CALL(model, time_steps()).WillRepeatedly(ReturnRef(time_steps));
//  EXPECT_CALL(model, years()).WillRepeatedly(Return(years));
//  EXPECT_CALL(model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));
//
//  selectivities::Logistic logistic(&model);
//  logistic.parameters().Add(PARAM_LABEL, "unit_test_logistic", __FILE__, __LINE__);
//  logistic.parameters().Add(PARAM_TYPE, "not needed in test", __FILE__, __LINE__);
//  logistic.parameters().Add(PARAM_A50,   "6",  __FILE__, __LINE__);
//  logistic.parameters().Add(PARAM_ATO95, "3",  __FILE__, __LINE__);
//  logistic.Validate();
//  logistic.Build();
//
//  partition::Category male(&model);
//  male.min_age_ = 5;
//  male.max_age_ = 10;
//  male.data_.assign((male.max_age_ - male.min_age_) + 1, 6000);
//
//  male.mean_length_by_time_step_age_[0][5] = 4.95960;
//  male.mean_length_by_time_step_age_[0][6] = 9.61173;
//  male.mean_length_by_time_step_age_[0][7] = 13.97545;
//  male.mean_length_by_time_step_age_[0][8] = 18.06864;
//  male.mean_length_by_time_step_age_[0][9] = 21.90808;
//  male.mean_length_by_time_step_age_[0][10] = 25.50949;
//
//  MockVonBertalanffy von_bertalanffy1(model, 80, 0.064, 4, false, 0.2, 0.2, {1.0});
//  ASSERT_NO_THROW(von_bertalanffy1.MockBuildCV());
//
//  von_bertalanffy1.DoAgeToLengthMatrixConversion(&male, {0, 10, 20, 25, 30}, true, &logistic);
//
//  // Check that the CV is being built appropriately and that the mean is stored correctly
//   EXPECT_DOUBLE_EQ(0.2, von_bertalanffy1.cv(1999, 0, 5));
//   EXPECT_DOUBLE_EQ(0.2, von_bertalanffy1.cv(1999, 0, 10));
//
//   //Run through length for the min and max age
//   vector<Double> expec1 = {1635.5889366403298, 0.00030682770114173025, 0, 0, 0};
//   vector<Double> expec2 = {6.9600494045431862, 817.35282114104973, 1883.6331656223699, 2061.666165823669, 1114.3311224993274};
//
//   unsigned age_index = 0;
//   for(unsigned bin = 0; bin <= 4; ++bin)
//     EXPECT_DOUBLE_EQ(expec1[bin], male.age_length_matrix_[age_index][bin]) << " where age = " << (male.min_age_ + age_index) << " where class_bin = " << bin;
//
//   age_index = 5;
//   for(unsigned bin = 0; bin <= 4; ++bin)
//     EXPECT_DOUBLE_EQ(expec2[bin], male.age_length_matrix_[age_index][bin]) << " where age = " << (male.min_age_ + age_index) << " where class_bin = " << bin;
//}


} /* namespace agelength */
} /* namespace niwa */
#endif

