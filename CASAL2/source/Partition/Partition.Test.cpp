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
    vector<string> age_lengths = {"no_age_length", "no_age_length", "no_age_length", "no_age_length"};

    for (string sex : sexes)
      for (string stage : stages) names.push_back(sex + "." + stage);

    set_block_type(PARAM_CATEGORIES);
    parameters().Add(PARAM_FORMAT, "sex.stage", __FILE__, __LINE__);
    parameters().Add(PARAM_NAMES, names, __FILE__, __LINE__);
    parameters().Add(PARAM_AGE_LENGTHS, age_lengths, __FILE__, __LINE__);

    
  };

  map<string, string> GetCategoryLabelsAndValues(const string& lookup, const string& parameter_location) override final {
    return Categories::GetCategoryLabelsAndValues(lookup, parameter_location);
  }

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

} /* namespace niwa */
#endif
