/**
 * @file Partition.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 01/03/2018
 * @section LICENSE
 *
 * Copyright NIWA Science �2018 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>

#include "../AgeLengths/Age/VonBertalanffy.h"
#include "../Categories/Categories.h"
#include "../Selectivities/Common/Logistic.h"
#include "../TestResources/MockClasses/Managers.h"
#include "../TestResources/MockClasses/Model.h"
#include "../TimeSteps/Manager.h"
#include "Partition.h"

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
    vector<string> sexes  = {"male", "female"};
    vector<string> stages = {"immature", "mature"};

    for (string sex : sexes)
      for (string stage : stages) names.push_back(sex + "." + stage);

    set_block_type(PARAM_CATEGORIES);
    parameters().Add(PARAM_FORMAT, "sex.stage", __FILE__, __LINE__);
    parameters().Add(PARAM_NAMES, names, __FILE__, __LINE__);
  };

  map<string, string> GetCategoryLabelsAndValues(const string& lookup, const string& parameter_location) override final {
    return Categories::GetCategoryLabelsAndValues(lookup, parameter_location);
  }

  AgeLength* age_length_ = nullptr;
  AgeLength* age_length(const string& category_name) override final { return age_length_; }
};

class MockVonBertalanffy : public agelengths::VonBertalanffy {
public:
  MockVonBertalanffy(Distribution distribution = Distribution::kNormal) : VonBertalanffy(nullptr) { distribution_ = distribution; }

  MockVonBertalanffy(shared_ptr<Model> model, Double linf, Double k, Double t0, bool by_length, Double cv_first, Double cv_last, vector<double> time_step_proportions,
                     string compatibility = PARAM_CASAL2, Distribution distribution = Distribution::kNormal) :
      VonBertalanffy(model) {
    linf_                  = linf;
    k_                     = k;
    t0_                    = t0;
    by_length_             = by_length;
    cv_first_              = cv_first;
    cv_last_               = cv_last;
    time_step_proportions_ = time_step_proportions;
    compatibility_         = compatibility;
    distribution_          = distribution;
    year_offset_ = model ->start_year();
    age_offset_ = model ->min_age();    
    model_years_ = model->years();
    min_age_ = model ->min_age(); 
    max_age_ = model ->min_age(); 
    // allocate memory for cvs; this is usually done in the Build() but difficult to mock
    cvs_.resize(model->years().size());
    for(unsigned year_ndx = 0; year_ndx < cvs_.size(); ++year_ndx) {
      cvs_[year_ndx].resize(model->time_steps().size());
      for(unsigned time_step_ndx = 0; time_step_ndx < cvs_[year_ndx].size(); ++time_step_ndx) {
        cvs_[year_ndx][time_step_ndx].resize(model->age_spread(), 0.0);
      }
    }   
  }

  void MockPopulateCV() { this->PopulateCV(); }
  void MockUpdateAgeLengthMatrixForThisYear(unsigned year) { this->UpdateAgeLengthMatrixForThisYear(year); }

  Double mean_weight(unsigned time_step, unsigned age) { return 0.0; }

  double override_cv_ = 0.0;
  Double cv(unsigned year, unsigned time_step, unsigned age)  {
    if (override_cv_ == 0.0)
      return cvs_[year - year_offset_][time_step - time_step_offset_][age - age_offset_];
    return override_cv_;
  };
};

class MockPartition : public niwa::Partition {
public:
  MockPartition(shared_ptr<Model> model) : Partition(model){};
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
  MockCategories        mock_categories(model);
  MockPartition         partition(model);
  MockTimeStepManager   time_step_manager;
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
  MockCategories        mock_categories(model);
  MockPartition         partition(model);
  MockTimeStepManager   time_step_manager;
  time_step_manager.time_step_index_ = 0;

  model->bind_calls();
  EXPECT_CALL(*model, categories()).WillRepeatedly(Return(&mock_categories));
  ASSERT_NO_THROW(mock_categories.Validate());

  MockVonBertalanffy von_bertalanffy(model, 80, 0.064, 4, false, 0.2, 0.2, {1.0, 1.0});
  ASSERT_NO_THROW(von_bertalanffy.MockPopulateCV());
  //ASSERT_NO_THROW(von_bertalanffy.MockUpdateAgeLengthMatrixForThisYear(model.start_year()));

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

  //ASSERT_NO_THROW(partition.BuildMeanLengthData());

  //ASSERT_EQ(3u, male_mature.mean_length_by_time_step_age_.size());
  ////ASSERT_EQ(2u, male_mature.mean_length_by_time_step_age_[0].size());
  //ASSERT_EQ(10u, male_mature.mean_length_by_time_step_age_[0][0].size());

  // Check our Von Bert age length is returning the right answers
  // note the second parameter is the age, not the index to the vector
  EXPECT_DOUBLE_EQ(0.0, von_bertalanffy.mean_length(0, 1));
  EXPECT_DOUBLE_EQ(0.0, von_bertalanffy.mean_length(0, 2));
  EXPECT_DOUBLE_EQ(0.0, von_bertalanffy.mean_length(0, 3));
  EXPECT_DOUBLE_EQ(4.9596000375416427, von_bertalanffy.mean_length(0, 4));
  EXPECT_DOUBLE_EQ(9.611729668428497, von_bertalanffy.mean_length(0, 5));
  EXPECT_DOUBLE_EQ(13.975450520665413, von_bertalanffy.mean_length(0, 6));
  EXPECT_DOUBLE_EQ(18.068642496620129, von_bertalanffy.mean_length(0, 7));
  EXPECT_DOUBLE_EQ(21.908077034104725, von_bertalanffy.mean_length(0, 8));
  EXPECT_DOUBLE_EQ(25.509485825636233, von_bertalanffy.mean_length(0, 9));
  EXPECT_DOUBLE_EQ(28.887625277446702, von_bertalanffy.mean_length(0, 10));

  // note all parameters are indexes now, not absolute values
  //EXPECT_DOUBLE_EQ(0.0, male_mature.mean_length_by_time_step_age_[0][0][0]);
  //EXPECT_DOUBLE_EQ(0.0, male_mature.mean_length_by_time_step_age_[0][0][1]);
  //EXPECT_DOUBLE_EQ(0.0, male_mature.mean_length_by_time_step_age_[0][0][2]);
  //EXPECT_DOUBLE_EQ(4.9596000375416427, male_mature.mean_length_by_time_step_age_[0][0][3]);
  //EXPECT_DOUBLE_EQ(9.611729668428497, male_mature.mean_length_by_time_step_age_[0][0][4]);
  //EXPECT_DOUBLE_EQ(13.975450520665413, male_mature.mean_length_by_time_step_age_[0][0][5]);
  //EXPECT_DOUBLE_EQ(18.068642496620129, male_mature.mean_length_by_time_step_age_[0][0][6]);
  //EXPECT_DOUBLE_EQ(21.908077034104725, male_mature.mean_length_by_time_step_age_[0][0][7]);
  //EXPECT_DOUBLE_EQ(25.509485825636233, male_mature.mean_length_by_time_step_age_[0][0][8]);
  //EXPECT_DOUBLE_EQ(28.887625277446702, male_mature.mean_length_by_time_step_age_[0][0][9]);
}

} /* namespace niwa */
#endif
