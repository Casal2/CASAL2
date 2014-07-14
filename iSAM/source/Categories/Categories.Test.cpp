/**
 * @file Categories.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 10/07/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "Categories.h"

#include <iostream>

#include "TestResources/TestFixtures/InternalEmptyModel.h"

// Namespaces
namespace isam {
namespace categories {

using std::cout;
using std::endl;
using isam::testfixtures::InternalEmptyModel;

/**
 *
 */
TEST_F(InternalEmptyModel, Categories_GetCategoryLabels) {
  CategoriesPtr categories = Categories::Instance();

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
} /* namespace isam */

#endif
