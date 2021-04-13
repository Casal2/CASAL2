/**
 * @file Partition.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 01/03/2018
 * @section LICENSE
 *
 * Copyright NIWA Science ©2018 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "Partition.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

#include "../AgeLengths/Age/VonBertalanffy.h"
#include "../Categories/Categories.h"
#include "../Selectivities/Common/Logistic.h"
#include "../TimeSteps/Manager.h"
#include "../TestResources/MockClasses/Managers.h"
#include "../TestResources/MockClasses/Model.h"

// namespaces
namespace niwa {
using ::testing::Return;
using ::testing::ReturnRef;

// classes
class MockTimeStepManager : public timesteps::Manager {
public:
  unsigned time_step_index_ = 0;
  unsigned current_time_step() const override final { return time_step_index_; }
};

class MockCategories : public Categories {
public:
  MockCategories(shared_ptr<Model> model) : Categories(model) {
    vector<string> names;
    vector<string> sexes  = { "male", "female" };
    vector<string> stages = { "immature", "mature" };

    for (string sex : sexes)
      for (string stage : stages)
        names.push_back(sex + "." + stage);

    set_block_type(PARAM_CATEGORIES);
    parameters().Add(PARAM_FORMAT, "sex.stage", __FILE__, __LINE__);
    parameters().Add(PARAM_NAMES, names, __FILE__, __LINE__);
  };

  map<string, string> GetCategoryLabelsAndValues(const string& lookup, const string& parameter_location) override final{
    return Categories::GetCategoryLabelsAndValues(lookup, parameter_location);
  }

  AgeLength* age_length_ = nullptr;
  AgeLength* age_length(const string& category_name) override final { return age_length_; }
  bool HasAgeLengths() const override final { return age_length_ != nullptr; }
};

class MockVonBertalanffy : public agelengths::VonBertalanffy {
public:
  MockVonBertalanffy(Distribution distribution = Distribution::kNormal)
  : VonBertalanffy(nullptr) {
    distribution_ = distribution;
  }

  MockVonBertalanffy(shared_ptr<Model> model, Double linf, Double k, Double t0, bool by_length,
      Double cv_first, Double cv_last, vector<double> time_step_proportions, bool casal_switch = false, Distribution distributuion = Distribution::kNormal) : VonBertalanffy(model) {
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

  Double mean_weight(unsigned time_step, unsigned age) override final { return 0.0; }

  double override_cv_ = 0.0;
  Double cv(unsigned year, unsigned time_step, unsigned age) override final {
    if (override_cv_ == 0.0)
      return cvs_[year][time_step][age];
    return override_cv_;
  };
};

class MockPartition : public niwa::Partition {
public:
  MockPartition(shared_ptr<Model> model) : Partition(model) {
  };
};

/**
 * This method will test our mock structures ensuring the Partition will
 * Validate and Build with some basic settings.
 *
 * This is to ensure we're loading values into our partition as we expect
 * so we can setup some more tests.
 */
TEST(Partition, ValidateAndBuild) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  MockCategories mock_categories(model);
  MockPartition partition(model);
  MockTimeStepManager time_step_manager;
  time_step_manager.time_step_index_ = 0;

  model->bind_calls();
  EXPECT_CALL(*model, categories()).WillRepeatedly(Return(&mock_categories));

  ASSERT_NO_THROW(mock_categories.Validate());
  ASSERT_EQ(4u, mock_categories.category_names().size());
  EXPECT_EQ("male.immature", mock_categories.category_names()[0]);
  EXPECT_EQ("male.mature", mock_categories.category_names()[1]);
  EXPECT_EQ("female.immature", mock_categories.category_names()[2]);
  EXPECT_EQ("female.mature", mock_categories.category_names()[3]);

  ASSERT_NO_THROW(partition.Validate());
  ASSERT_NO_THROW(partition.Build());

  ASSERT_NO_THROW(partition.category("male.mature"));
  ASSERT_NO_THROW(partition.category("female.immature"));

  auto& category = partition.category("male.mature");
  EXPECT_EQ(category.min_age_, 1u);
  EXPECT_EQ(category.max_age_, 10u);
  EXPECT_EQ(nullptr, category.age_length_);
}

/**
 * This method will test the X method
 */
TEST(Partition, BuildMeanLength) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  MockCategories mock_categories(model);
  MockPartition partition(model);
  MockTimeStepManager time_step_manager;
  time_step_manager.time_step_index_ = 0;

  model->bind_calls();
  EXPECT_CALL(*model, categories()).WillRepeatedly(Return(&mock_categories));

  ASSERT_NO_THROW(mock_categories.Validate());

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, 4, false, 0.2, 0.2, {1.0});
  ASSERT_NO_THROW(von_bertalanffy.MockBuildCV());
  ASSERT_NO_THROW(von_bertalanffy.RebuildCache());
  mock_categories.age_length_ = &von_bertalanffy;

  ASSERT_NO_THROW(partition.Validate());
  ASSERT_NO_THROW(partition.Build());

  ASSERT_NO_THROW(partition.category("male.mature"));
  ASSERT_NO_THROW(partition.category("female.immature"));

  auto& male_mature = partition.category("male.mature");
  EXPECT_EQ(male_mature.min_age_, 1u);
  EXPECT_EQ(male_mature.max_age_, 10u);
  EXPECT_NE(nullptr, male_mature.age_length_);

  ASSERT_NO_THROW(partition.BuildMeanLengthData());

  ASSERT_EQ(3u, male_mature.mean_length_by_time_step_age_.size());
  ASSERT_EQ(2u, male_mature.mean_length_by_time_step_age_[0].size());
  ASSERT_EQ(10u, male_mature.mean_length_by_time_step_age_[0][0].size());

  // Check our Von Bert age length is returnining the right answers
  // note the second parameter is the age, not the index to the vector
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

  // note all parameters are indexes now, not absolute values
  EXPECT_DOUBLE_EQ(0.0, male_mature.mean_length_by_time_step_age_[0][0][0]);
  EXPECT_DOUBLE_EQ(0.0, male_mature.mean_length_by_time_step_age_[0][0][1]);
  EXPECT_DOUBLE_EQ(0.0, male_mature.mean_length_by_time_step_age_[0][0][2]);
  EXPECT_DOUBLE_EQ(4.9596000375416427, male_mature.mean_length_by_time_step_age_[0][0][3]);
  EXPECT_DOUBLE_EQ(9.611729668428497,  male_mature.mean_length_by_time_step_age_[0][0][4]);
  EXPECT_DOUBLE_EQ(13.975450520665413, male_mature.mean_length_by_time_step_age_[0][0][5]);
  EXPECT_DOUBLE_EQ(18.068642496620129, male_mature.mean_length_by_time_step_age_[0][0][6]);
  EXPECT_DOUBLE_EQ(21.908077034104725, male_mature.mean_length_by_time_step_age_[0][0][7]);
  EXPECT_DOUBLE_EQ(25.509485825636233, male_mature.mean_length_by_time_step_age_[0][0][8]);
  EXPECT_DOUBLE_EQ(28.887625277446702, male_mature.mean_length_by_time_step_age_[0][0][9]);
}

/**
 * This method will test the X method
 */
TEST(Partition, BuildAgeLengthProportions) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  MockCategories mock_categories(model);
  MockPartition partition(model);
  MockTimeStepManager time_step_manager;
  time_step_manager.time_step_index_ = 0;

  model->set_length_plus(true);
  model->bind_calls();

  EXPECT_CALL(*model, categories()).WillRepeatedly(Return(&mock_categories));

  ASSERT_NO_THROW(mock_categories.Validate());

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, 4, false, 0.2, 0.2, {1.0});
  ASSERT_NO_THROW(von_bertalanffy.MockBuildCV());
  ASSERT_NO_THROW(von_bertalanffy.RebuildCache());
  mock_categories.age_length_ = &von_bertalanffy;

  ASSERT_NO_THROW(partition.Validate());
  ASSERT_NO_THROW(partition.Build());

  auto& male_mature = partition.category("male.mature");
  EXPECT_EQ(male_mature.min_age_, 1u);
  EXPECT_EQ(male_mature.max_age_, 10u);
  EXPECT_NE(nullptr, male_mature.age_length_);

  ASSERT_NO_THROW(partition.BuildMeanLengthData());

  ASSERT_EQ(3u, male_mature.mean_length_by_time_step_age_.size());
  ASSERT_EQ(2u, male_mature.mean_length_by_time_step_age_[0].size());
  ASSERT_EQ(10u, male_mature.mean_length_by_time_step_age_[0][0].size());

  ASSERT_NO_THROW(partition.BuildAgeLengthProportions());
  ASSERT_NO_THROW(partition.age_length_proportions("male.immature"));

  auto& male_age_props = partition.age_length_proportions("male.immature");
  // vector<year, time_step, age, length, proportion>>;
  ASSERT_EQ(3u, male_age_props.size());
  ASSERT_EQ(2u, male_age_props[0].size());
  ASSERT_EQ(10u, male_age_props[0][0].size());
  ASSERT_EQ(5u, male_age_props[0][0][0].size());

  for (unsigned year = 0; year < 1; ++year) {
  EXPECT_DOUBLE_EQ(0u, male_age_props[year][0][0][0]);
  EXPECT_DOUBLE_EQ(0u, male_age_props[year][0][0][1]);
  EXPECT_DOUBLE_EQ(0u, male_age_props[year][0][0][2]);
  EXPECT_DOUBLE_EQ(0u, male_age_props[year][0][0][3]);
  EXPECT_DOUBLE_EQ(0u, male_age_props[year][0][0][4]);

  EXPECT_DOUBLE_EQ(0u, male_age_props[year][0][2][0]);
  EXPECT_DOUBLE_EQ(0u, male_age_props[year][0][2][1]);
  EXPECT_DOUBLE_EQ(0u, male_age_props[year][0][2][2]);
  EXPECT_DOUBLE_EQ(0u, male_age_props[year][0][2][3]);
  EXPECT_DOUBLE_EQ(0u, male_age_props[year][0][2][4]);

  EXPECT_DOUBLE_EQ(0.41996720731752746,     male_age_props[year][0][4][0]);
  EXPECT_DOUBLE_EQ(3.2667977767353307e-008, male_age_props[year][0][4][1]);
  EXPECT_DOUBLE_EQ(0u, male_age_props[year][0][4][2]);
  EXPECT_DOUBLE_EQ(0u, male_age_props[year][0][4][3]);
  EXPECT_DOUBLE_EQ(0u, male_age_props[year][0][4][4]);

  EXPECT_DOUBLE_EQ(0.69070308538891911,      male_age_props[year][0][6][0]);
  EXPECT_DOUBLE_EQ(0.29603445809402762,      male_age_props[year][0][6][1]);
  EXPECT_DOUBLE_EQ(0.00048060422206530617,   male_age_props[year][0][6][2]);
  EXPECT_DOUBLE_EQ(6.4636329621947652e-010,  male_age_props[year][0][6][3]);
  EXPECT_DOUBLE_EQ(0u, male_age_props[0][0][6][4]);

  EXPECT_DOUBLE_EQ(0.13891252421751288, male_age_props[year][0][8][0]);
  EXPECT_DOUBLE_EQ(0.67051952644369806, male_age_props[year][0][8][1]);
  EXPECT_DOUBLE_EQ(0.18713059299787771, male_age_props[year][0][8][2]);
  EXPECT_DOUBLE_EQ(0.0022533864694446182, male_age_props[year][0][8][3]);
  EXPECT_DOUBLE_EQ(7.9325922641704238e-007, male_age_props[year][0][8][4]);
  }
}

/**
 * This method will test the BuildAgeLengthProportions  method
 */
TEST(Partition, BuildAgeLengthProportions_2) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  MockCategories mock_categories(model);
  MockPartition partition(model);
  MockTimeStepManager time_step_manager;
  time_step_manager.time_step_index_ = 0;

  model->set_length_plus(true);
  model->set_length_bins({0, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 ,32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47});
  model->bind_calls();

  EXPECT_CALL(*model, categories()).WillRepeatedly(Return(&mock_categories));
  ASSERT_NO_THROW(mock_categories.Validate());

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, 4, false, 0.2, 0.2, {1.0});
  ASSERT_NO_THROW(von_bertalanffy.MockBuildCV());
  ASSERT_NO_THROW(von_bertalanffy.RebuildCache());
  mock_categories.age_length_ = &von_bertalanffy;

  ASSERT_NO_THROW(partition.Validate());
  ASSERT_NO_THROW(partition.Build());

  auto& male_mature = partition.category("male.mature");
  EXPECT_EQ(male_mature.min_age_, 1u);
  EXPECT_EQ(male_mature.max_age_, 10u);
  EXPECT_NE(nullptr, male_mature.age_length_);

  ASSERT_NO_THROW(partition.BuildMeanLengthData());

  ASSERT_EQ(3u, male_mature.mean_length_by_time_step_age_.size());
  ASSERT_EQ(2u, male_mature.mean_length_by_time_step_age_[0].size());
  ASSERT_EQ(10u, male_mature.mean_length_by_time_step_age_[0][0].size());

  double mu = 35.49858;
  for (unsigned i = 0; i < male_mature.mean_length_by_time_step_age_.size(); ++i)
    for (unsigned j = 0; j < male_mature.mean_length_by_time_step_age_[i].size(); ++j)
      for (unsigned k = 0; k < male_mature.mean_length_by_time_step_age_[i][j].size(); ++k)
        male_mature.mean_length_by_time_step_age_[i][j][k] = mu;
  von_bertalanffy.override_cv_ = 0.1;

  ASSERT_NO_THROW(partition.BuildAgeLengthProportions());
  ASSERT_NO_THROW(partition.age_length_proportions("male.mature"));

  auto& male_age_props = partition.age_length_proportions("male.mature");
  // vector<year, time_step, age, length, proportion>>;
  ASSERT_EQ(3u, male_age_props.size());
  ASSERT_EQ(2u, male_age_props[0].size());
  ASSERT_EQ(10u, male_age_props[0][0].size());
  ASSERT_EQ(34u, male_age_props[0][0][0].size());

  vector<Double> expected = {3.8713535710499514e-009, 1.5960216925847703e-008, 7.422358561104403e-008, 3.1901955588331532e-007, 1.2672619864595447e-006, 4.6525401673491729e-006,
      1.5786604316003761e-005, 4.9506445653380027e-005,0.00014348551812060073, 0.00038434913282614502,0.00095150900849361175, 0.0021770396325317964, 0.0046034492460040877,
      0.0089962477651120976, 0.016247952527619458, 0.027120294871666895, 0.041835938013406682, 0.059643893860880981, 0.078586144395677904, 0.095695106434901311,0.10769489117184539,
      0.11201216057229546,0.10767078378048922,0.095652266453105428,0.078533378916068819, 0.059590504250288112, 0.041789131783027234 , 0.027083887651074501, 0.01622250783484136,
      0.0089801483958439343, 0.0045941822881506722, 0.002172170763177772, 0.00094916847523229819, 0.00059778133048304927};

  ASSERT_EQ(expected.size(), male_age_props[0][0][0].size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected[i], male_age_props[0][0][0][i]) << " with i = " << i;
  }
}

/**
 * This method will test the BuildAgeLengthProportions  method
 */
TEST(Partition, BuildAgeLengthProportions_3) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  MockCategories mock_categories(model);
  MockPartition partition(model);
  MockTimeStepManager time_step_manager;
  time_step_manager.time_step_index_ = 0;

  model->set_length_bins({0, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 ,32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47});
  model->bind_calls();
  EXPECT_CALL(*model, categories()).WillRepeatedly(Return(&mock_categories));
  ASSERT_NO_THROW(mock_categories.Validate());

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, 4, false, 0.2, 0.2, {1.0}, false, Distribution::kLogNormal);
  ASSERT_NO_THROW(von_bertalanffy.MockBuildCV());
  ASSERT_NO_THROW(von_bertalanffy.RebuildCache());
  mock_categories.age_length_ = &von_bertalanffy;

  ASSERT_NO_THROW(partition.Validate());
  ASSERT_NO_THROW(partition.Build());

  auto& male_mature = partition.category("male.mature");
  EXPECT_EQ(male_mature.min_age_, 1u);
  EXPECT_EQ(male_mature.max_age_, 10u);
  EXPECT_NE(nullptr, male_mature.age_length_);

  ASSERT_NO_THROW(partition.BuildMeanLengthData());

  ASSERT_EQ(3u, male_mature.mean_length_by_time_step_age_.size());
  ASSERT_EQ(2u, male_mature.mean_length_by_time_step_age_[0].size());
  ASSERT_EQ(10u, male_mature.mean_length_by_time_step_age_[0][0].size());

  double mu = 35.49858;
  for (unsigned i = 0; i < male_mature.mean_length_by_time_step_age_.size(); ++i)
    for (unsigned j = 0; j < male_mature.mean_length_by_time_step_age_[i].size(); ++j)
      for (unsigned k = 0; k < male_mature.mean_length_by_time_step_age_[i][j].size(); ++k)
        male_mature.mean_length_by_time_step_age_[i][j][k] = mu;
  von_bertalanffy.override_cv_ = 0.1;

  ASSERT_NO_THROW(partition.BuildAgeLengthProportions());
  ASSERT_NO_THROW(partition.age_length_proportions("male.mature"));

  auto& male_age_props = partition.age_length_proportions("male.mature");
  // vector<year, time_step, age, length, proportion>>;
  ASSERT_EQ(3u, male_age_props.size());
  ASSERT_EQ(2u, male_age_props[0].size());
  ASSERT_EQ(10u, male_age_props[0][0].size());
  ASSERT_EQ(33u, male_age_props[0][0][0].size());

  vector<Double> expected = {0, 9.9920072216264089e-016,1.1390888232654106e-013, 6.907807659217724e-012, 2.4863089365112501e-010, 5.6808661108576075e-009, 8.7191919018181352e-008,
      9.4269457673323842e-007, 7.4745056608538363e-006, 4.4982380957292456e-005, 0.00021163731992057677, 0.00079862796125962365, 0.0024715534075264722, 0.0063962867724943751,0.01408161729231916,
      0.026773528172936767, 0.044555539731829574,  0.065676687795628297, 0.086665248035340148,0.1033532304575121, 0.11234199264093081, 0.11215706824927596, 0.10355520403577334,
      0.088978990579101747, 0.071552403002032916, 0.054126600808507175, 0.038696543948670059, 0.026257468168220055, 0.016976053665368585, 0.010494572238876954, 0.0062237099943515117,
      0.0035513001369547048,0.0019551097231892411 };

  ASSERT_EQ(expected.size(), male_age_props[0][0][0].size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected[i], male_age_props[0][0][0][i]) << " with i = " << i;
  }
}

/**
 * This method will test the BuildAgeLengthProportions  method
 */
TEST(Partition, BuildAgeLengthProportions_4) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  MockCategories mock_categories(model);
  MockPartition partition(model);
  MockTimeStepManager time_step_manager;
  time_step_manager.time_step_index_ = 0;

  model->set_length_bins({0, 20, 40, 60 , 80, 110});
  model->bind_calls();
  EXPECT_CALL(*model, categories()).WillRepeatedly(Return(&mock_categories));
  ASSERT_NO_THROW(mock_categories.Validate());

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, 4, false, 0.2, 0.2, {1.0});
  ASSERT_NO_THROW(von_bertalanffy.MockBuildCV());
  ASSERT_NO_THROW(von_bertalanffy.RebuildCache());
  mock_categories.age_length_ = &von_bertalanffy;

  ASSERT_NO_THROW(partition.Validate());
  ASSERT_NO_THROW(partition.Build());

  auto& male_mature = partition.category("male.mature");
  EXPECT_EQ(male_mature.min_age_, 1u);
  EXPECT_EQ(male_mature.max_age_, 10u);
  EXPECT_NE(nullptr, male_mature.age_length_);

  ASSERT_NO_THROW(partition.BuildMeanLengthData());

  ASSERT_EQ(3u, male_mature.mean_length_by_time_step_age_.size());
  ASSERT_EQ(2u, male_mature.mean_length_by_time_step_age_[0].size());
  ASSERT_EQ(10u, male_mature.mean_length_by_time_step_age_[0][0].size());

  double mu = 40.081628;
  for (unsigned i = 0; i < male_mature.mean_length_by_time_step_age_.size(); ++i)
    for (unsigned j = 0; j < male_mature.mean_length_by_time_step_age_[i].size(); ++j)
      for (unsigned k = 0; k < male_mature.mean_length_by_time_step_age_[i][j].size(); ++k)
        male_mature.mean_length_by_time_step_age_[i][j][k] = mu;
  von_bertalanffy.override_cv_ = 0.1;

  ASSERT_NO_THROW(partition.BuildAgeLengthProportions());
  ASSERT_NO_THROW(partition.age_length_proportions("male.mature"));

  auto& male_age_props = partition.age_length_proportions("male.mature");
  // vector<year, time_step, age, length, proportion>>;
  ASSERT_EQ(3u, male_age_props.size());
  ASSERT_EQ(2u, male_age_props[0].size());
  ASSERT_EQ(10u, male_age_props[0][0].size());
  ASSERT_EQ(5u, male_age_props[0][0][0].size());

  vector<Double> expected = {2.7232626398365767e-007, 0.49187561267029634, 0.50812377877585069, 3.3622758899287675e-007,0};

  ASSERT_EQ(expected.size(), male_age_props[0][0][0].size());
  for (unsigned i = 0; i < expected.size(); ++i) {
    EXPECT_DOUBLE_EQ(expected[i], male_age_props[0][0][0][i]) << " with i = " << i;
  }
}
} /* namespace niwa */
#endif




