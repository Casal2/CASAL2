/**
 * @file Categories.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 10/07/2014
 * @section LICENSE
 *
 * Copyright NIWA Science �2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "Categories.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestResources/MockClasses/Model.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace niwa {
namespace categories {

using std::cout;
using std::endl;
using niwa::testfixtures::InternalEmptyModel;
using ::testing::Return;
using ::testing::ReturnRef;

/**
 * Mock Class Definitions
 */
class MockCategories : public Categories {
public:
  MockCategories(Model* model) : Categories(model) {
    vector<string> names;
    vector<string> sexes  = { "male", "female" };
    vector<string> stages = { "immature", "mature" };
    vector<string> tags   = { "notag", "2000", "2001", "2002" };

    for (string sex : sexes)
      for (string stage : stages)
        for (string tag : tags)
          names.push_back(sex + "." + stage + "." + tag);

    set_block_type(PARAM_CATEGORIES);
    parameters().Add(PARAM_FORMAT, "sex.stage.tag", __FILE__, __LINE__);
    parameters().Add(PARAM_NAMES, names, __FILE__, __LINE__);
  };

  map<string, string> GetCategoryLabelsAndValues(const string& lookup, const Parameter* source_parameter) override final{
    return Categories::GetCategoryLabelsAndValues(lookup, source_parameter);
  }
};

/**
 * Test validating the categories with a couple of year values to ensure it works.
 */
TEST_F(InternalEmptyModel, Categories_AssignSpecificYearsPerCategory_1) {
  MockModel model;
  vector<unsigned> model_years { 2000u, 2001u, 2002u, 2003u, 2004u };
  EXPECT_CALL(model, years()).WillRepeatedly(Return(model_years));
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(model, start_year()).WillRepeatedly(Return(2000));
  EXPECT_CALL(model, final_year()).WillRepeatedly(Return(2010));

  MockCategories categories(&model);

  vector<string> years_lookup = {"sex=male=2000,2002,2004"};
  categories.parameters().Add(PARAM_YEARS, years_lookup, __FILE__, __LINE__);
  ASSERT_NO_THROW(categories.Validate());

  auto years = categories.years("male.immature.notag");
  ASSERT_EQ(3u, years.size());
  EXPECT_EQ(2000u, years[0]);
  EXPECT_EQ(2002u, years[1]);
  EXPECT_EQ(2004u, years[2]);
}

/**
 * Check that we get an appropriate error when we re-define a year
 * more than once
 */
TEST_F(InternalEmptyModel, Categories_AssignSpecificYearsPerCategory_2_ExpectError) {
  MockModel model;
  vector<unsigned> model_years { 2000u, 2001u, 2002u, 2003u };
  EXPECT_CALL(model, years()).WillRepeatedly(Return(model_years));
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(model, start_year()).WillRepeatedly(Return(2000));
  EXPECT_CALL(model, final_year()).WillRepeatedly(Return(2010));

  MockCategories categories(&model);

  vector<string> years_lookup = {"sex=male=2000,2000,2002"};
  categories.parameters().Add(PARAM_YEARS, years_lookup, __FILE__, __LINE__);
  ASSERT_THROW(categories.Validate(), std::string);
}

/**
 * Check that we can use a wildcard lookup
 */
TEST_F(InternalEmptyModel, Categories_AssignSpecificYearsPerCategory_3) {
  MockModel model;
  vector<unsigned> model_years { 2000u, 2001u, 2002u, 2003u };
  EXPECT_CALL(model, years()).WillRepeatedly(Return(model_years));
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(model, start_year()).WillRepeatedly(Return(2000));
  EXPECT_CALL(model, final_year()).WillRepeatedly(Return(2010));

  MockCategories categories(&model);

  vector<string> years_lookup = {"tag=*=2002"};
  categories.parameters().Add(PARAM_YEARS, years_lookup, __FILE__, __LINE__);
  ASSERT_NO_THROW(categories.Validate());

  auto years = categories.years("male.immature.notag");
  ASSERT_EQ(1u, years.size());
  EXPECT_EQ(2002u, years[0]);
}


/**
 * Check that we can specify just a category
 */
TEST_F(InternalEmptyModel, Categories_AssignSpecificYearsPerCategory_4) {
  MockModel model;
  vector<unsigned> model_years { 2000u, 2001u, 2002u, 2003u };
  EXPECT_CALL(model, years()).WillRepeatedly(Return(model_years));
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(model, start_year()).WillRepeatedly(Return(2000));
  EXPECT_CALL(model, final_year()).WillRepeatedly(Return(2010));

  MockCategories categories(&model);

  vector<string> years_lookup = {"male.immature.notag=2002"};
  categories.parameters().Add(PARAM_YEARS, years_lookup, __FILE__, __LINE__);
  ASSERT_NO_THROW(categories.Validate());

  auto years = categories.years("male.immature.notag");
  ASSERT_EQ(1u, years.size());
  EXPECT_EQ(2002u, years[0]);
}

/**
 * This test will assign different years to different categories
 */
TEST_F(InternalEmptyModel, Categories_AssignSpecificYearsPerCategory) {
  MockModel model;
  vector<unsigned> model_years { 2000u, 2001u, 2002u, 2003u };
  EXPECT_CALL(model, years()).WillRepeatedly(Return(model_years));
  EXPECT_CALL(model, min_age()).WillRepeatedly(Return(10));
  EXPECT_CALL(model, max_age()).WillRepeatedly(Return(20));
  EXPECT_CALL(model, start_year()).WillRepeatedly(Return(2000));
  EXPECT_CALL(model, final_year()).WillRepeatedly(Return(2010));

  MockCategories categories(&model);
  categories.Validate();

  /**
   * Check 1: Assigning a value to all categories
   */
  string lookup = "*=2000,2001";
  auto results = categories.GetCategoryLabelsAndValues(lookup, categories.parameters().Get(PARAM_NAMES));
  EXPECT_EQ(16u, results.size());
  EXPECT_EQ("2000,2001", results["female.mature.notag"]);
  EXPECT_EQ("2000,2001", results["male.immature.notag"]);

  /**
   * Check 2:
   * format=male.*.*
   */
  lookup = "format=male.*.*=2000,2003,2005";
  results = categories.GetCategoryLabelsAndValues(lookup, categories.parameters().Get(PARAM_NAMES));
  EXPECT_EQ(8u, results.size());
  EXPECT_EQ("2000,2003,2005", results["male.immature.notag"]);
  EXPECT_EQ("2000,2003,2005", results["male.immature.2000"]);

  /**
   * Check 3:
   * format=female.*.*
   */
  lookup = "format=female.*.*=2000,2003,2005";
  results = categories.GetCategoryLabelsAndValues(lookup, categories.parameters().Get(PARAM_NAMES));
  EXPECT_EQ(8u, results.size());
  EXPECT_EQ("2000,2003,2005", results["female.immature.notag"]);
  EXPECT_EQ("2000,2003,2005", results["female.immature.2000"]);

  /**
   * Check 4:
   * format=male.immature.*
   */
  lookup = "format=male.immature.*=2000";
  results = categories.GetCategoryLabelsAndValues(lookup, categories.parameters().Get(PARAM_NAMES));
  EXPECT_EQ(4u, results.size());
  EXPECT_EQ("2000", results["male.immature.notag"]);
  EXPECT_EQ("2000", results["male.immature.2000"]);

  /**
   * Check 5
   * format=male.*.notag
   */
  lookup = "format=male.*.notag=2000,2002,2004";
  results = categories.GetCategoryLabelsAndValues(lookup, categories.parameters().Get(PARAM_NAMES));
  EXPECT_EQ(2u, results.size());
  EXPECT_EQ("2000,2002,2004", results["male.immature.notag"]);
  EXPECT_EQ("2000,2002,2004", results["male.mature.notag"]);

  /**
   * Check 6
   * format=male.immaure.2000,2001
   */
  lookup = "format=male.immature.2000,2001=2002,2003";
  results = categories.GetCategoryLabelsAndValues(lookup, categories.parameters().Get(PARAM_NAMES));
  EXPECT_EQ(2u, results.size());
  EXPECT_EQ("2002,2003", results["male.immature.2000"]);
  EXPECT_EQ("2002,2003", results["male.immature.2001"]);

  /**
   * Check 7
   * sex=male
   */
  lookup = "sex=male=2005";
  results = categories.GetCategoryLabelsAndValues(lookup, categories.parameters().Get(PARAM_NAMES));
  EXPECT_EQ(8u, results.size());
  EXPECT_EQ("2005", results["male.immature.2000"]);
  EXPECT_EQ("2005", results["male.mature.2001"]);

  /**
   * Check 8
   * tag=notag
   */
  lookup = "tag=notag=2007";
  results = categories.GetCategoryLabelsAndValues(lookup, categories.parameters().Get(PARAM_NAMES));
  EXPECT_EQ(4u, results.size());
  EXPECT_EQ("2007", results["male.immature.notag"]);
  EXPECT_EQ("2007", results["male.mature.notag"]);

  /**
   * Check 9
   * tag=notag,2001
   */
  lookup = "tag=notag,2001=2009";
  results = categories.GetCategoryLabelsAndValues(lookup, categories.parameters().Get(PARAM_NAMES));
  EXPECT_EQ(8u, results.size());
  EXPECT_EQ("2009", results["male.immature.notag"]);
  EXPECT_EQ("2009", results["male.mature.notag"]);
  EXPECT_EQ("2009", results["male.immature.2001"]);
  EXPECT_EQ("2009", results["male.mature.2001"]);
}

/**
 *
 */
TEST_F(InternalEmptyModel, Categories_GetCategoryLabels) {
  Categories* categories = model_->categories();

  vector<string> names;

  vector<string> sexes  = { "male", "female" };
  vector<string> stages = { "immature", "mature" };
  vector<string> tags   = { "notag", "2000", "2001", "2002" };

  for (string sex : sexes)
    for (string stage : stages)
      for (string tag : tags)
        names.push_back(sex + "." + stage + "." + tag);

  categories->set_block_type(PARAM_CATEGORIES);
  categories->parameters().Add(PARAM_FORMAT, "sex.stage.tag", __FILE__, __LINE__);
  categories->parameters().Add(PARAM_NAMES, names, __FILE__, __LINE__);
  categories->Validate();

  /**
   * male.immature.notag male.immature.2000 male.immature.2001 male.immature.2002 male.mature.notag male.mature.2000 male.mature.2001 male.mature.2002 female.immature.notag female.immature.2000 female.immature.2001 female.immature.2002 female.mature.notag female.mature.2000 female.mature.2001 female.mature.2002
   * male.immature.notag+male.immature.2000+male.immature.2001+male.immature.2002+male.mature.notag+male.mature.2000+male.mature.2001+male.mature.2002+female.immature.notag+female.immature.2000+female.immature.2001+female.immature.2002+female.mature.notag+female.mature.2000+female.mature.2001+female.mature.2002
   */

  /*
   * Check 1:
   * *
   */
  string result = "";
  string lookup_string = "*";
  string expected = "male.immature.notag male.immature.2000 male.immature.2001 male.immature.2002 male.mature.notag male.mature.2000 male.mature.2001 male.mature.2002 female.immature.notag female.immature.2000 female.immature.2001 female.immature.2002 female.mature.notag female.mature.2000 female.mature.2001 female.mature.2002";

  result = categories->GetCategoryLabels(lookup_string, categories->parameters().Get(PARAM_NAMES));
  EXPECT_EQ(expected, result);

  /**
   * Check 2
   * *+
   */
  lookup_string = "*+";
  expected = "male.immature.notag+male.immature.2000+male.immature.2001+male.immature.2002+male.mature.notag+male.mature.2000+male.mature.2001+male.mature.2002+female.immature.notag+female.immature.2000+female.immature.2001+female.immature.2002+female.mature.notag+female.mature.2000+female.mature.2001+female.mature.2002";

  result = categories->GetCategoryLabels(lookup_string, categories->parameters().Get(PARAM_NAMES));
  EXPECT_EQ(expected, result);

  /**
   * Check 3
   * format=male.*.*
   */
  lookup_string = "format=male.*.*";
  expected = "male.immature.notag male.immature.2000 male.immature.2001 male.immature.2002 male.mature.notag male.mature.2000 male.mature.2001 male.mature.2002";

  result = categories->GetCategoryLabels(lookup_string, categories->parameters().Get(PARAM_NAMES));
  EXPECT_EQ(expected, result);

  /**
   * Check 4
   * format=female.*.*
   */
  lookup_string = "format=female.*.*";
  expected = "female.immature.notag female.immature.2000 female.immature.2001 female.immature.2002 female.mature.notag female.mature.2000 female.mature.2001 female.mature.2002";

  result = categories->GetCategoryLabels(lookup_string, categories->parameters().Get(PARAM_NAMES));
  EXPECT_EQ(expected, result);

  /**
   * Check 5
   * format=male.*.*+
   */
  lookup_string = "format=male.*.*+";
  expected = "male.immature.notag+male.immature.2000+male.immature.2001+male.immature.2002+male.mature.notag+male.mature.2000+male.mature.2001+male.mature.2002";

  result = categories->GetCategoryLabels(lookup_string, categories->parameters().Get(PARAM_NAMES));
  EXPECT_EQ(expected, result);

  /**
   * Check 6
   * format=male.immature.*
   */
  lookup_string = "format=male.immature.*+";
  expected = "male.immature.notag+male.immature.2000+male.immature.2001+male.immature.2002";

  result = categories->GetCategoryLabels(lookup_string, categories->parameters().Get(PARAM_NAMES));
  EXPECT_EQ(expected, result);

  /**
   * Check 7
   * format=*.immature.*
   */
  lookup_string = "format=*.immature.*";
  expected = "male.immature.notag male.immature.2000 male.immature.2001 male.immature.2002 female.immature.notag female.immature.2000 female.immature.2001 female.immature.2002";

  result = categories->GetCategoryLabels(lookup_string, categories->parameters().Get(PARAM_NAMES));
  EXPECT_EQ(expected, result);

  /**
   * Check 8
   * format=male.*.notag+
   */
  lookup_string = "format=male.*.notag+";
  expected = "male.immature.notag+male.mature.notag";

  result = categories->GetCategoryLabels(lookup_string, categories->parameters().Get(PARAM_NAMES));
  EXPECT_EQ(expected, result);

  /**
   * Check 9
   * format=male.immaure.2000,2001
   */
  lookup_string = "format=male.immature.2000,2001";
  expected = "male.immature.2000 male.immature.2001";

  result = categories->GetCategoryLabels(lookup_string, categories->parameters().Get(PARAM_NAMES));
  EXPECT_EQ(expected, result);

  /**
   * Check 10
   * format=male,female.*.2001,2002
   */
  lookup_string = "format=male,female.*.2001,2002";
  expected = "male.immature.2001 male.immature.2002 male.mature.2001 male.mature.2002 female.immature.2001 female.immature.2002 female.mature.2001 female.mature.2002";

  result = categories->GetCategoryLabels(lookup_string, categories->parameters().Get(PARAM_NAMES));
  EXPECT_EQ(expected, result);

  /**
   * Check 11 - Spaces in format
   * format=male . * . notag
   */
  lookup_string = "format=male . * . notag";
  expected = "male.immature.notag male.mature.notag";

  result = categories->GetCategoryLabels(lookup_string, categories->parameters().Get(PARAM_NAMES));
  EXPECT_EQ(expected, result);

  /**
   * Check 12 - Shortest Format
   * sex=male
   */
  lookup_string = "sex=male";
  expected = "male.immature.notag male.immature.2000 male.immature.2001 male.immature.2002 male.mature.notag male.mature.2000 male.mature.2001 male.mature.2002";

  result = categories->GetCategoryLabels(lookup_string, categories->parameters().Get(PARAM_NAMES));
  EXPECT_EQ(expected, result);

  /**
   * Check 13 - Shortest format tag
   * tag=notag
   */
  lookup_string = "tag=notag";
  expected = "male.immature.notag male.mature.notag female.immature.notag female.mature.notag";

  result = categories->GetCategoryLabels(lookup_string, categories->parameters().Get(PARAM_NAMES));
  EXPECT_EQ(expected, result);

  /**
   * Check 14 - Shortest format tag=notag,2001
   * tag=notag,2001
   */
  lookup_string = "tag=notag,2001";
  expected = "male.immature.notag male.immature.2001 male.mature.notag male.mature.2001 female.immature.notag female.immature.2001 female.mature.notag female.mature.2001";

  result = categories->GetCategoryLabels(lookup_string, categories->parameters().Get(PARAM_NAMES));
  EXPECT_EQ(expected, result);

  /**
   * Check 15 - Shortest format tag=notag,2001 with+
   * tag=notag,2001+
   */
  lookup_string = "tag=notag,2001+";
  expected = "male.immature.notag+male.immature.2001+male.mature.notag+male.mature.2001+female.immature.notag+female.immature.2001+female.mature.notag+female.mature.2001";

  result = categories->GetCategoryLabels(lookup_string, categories->parameters().Get(PARAM_NAMES));
  EXPECT_EQ(expected, result);
}

} /* namespace derivedquantities */
} /* namespace niwa */

#endif
