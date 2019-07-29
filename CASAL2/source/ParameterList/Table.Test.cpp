/**
 * @file Table.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 11/10/2018
 * @section LICENSE
 *
 * Copyright NIWA Science ©2018 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "Table.h"

#include <iostream>

#include "Categories/Categories.h"
#include "Model/Factory.h"
#include "TimeSteps/Manager.h"
#include "Partition/Partition.h"
#include "TestResources/TestFixtures/BasicModel.h"
#include "TestResources/MockClasses/Managers.h"
#include "TestResources/MockClasses/Model.h"

// Namespaces
namespace niwa {
namespace parameters {

using std::cout;
using std::endl;
using ::testing::NiceMock;

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

  map<string, string> GetCategoryLabelsAndValues(const string& lookup, const string& parameter_location) override final{
    return Categories::GetCategoryLabelsAndValues(lookup, parameter_location);
  }
};


/**
 * Check that we can create a partition table and add columns without error
 */
TEST(Parameters, Table_Create) {
  MockModel model;

  vector<string> columns = {"col1", "col2", "col3"};

  Table table("my_table");
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.Populate(&model));
}

/**
 * Check that populate fails with a bad model pointers
 */
TEST(Parameters, Table_Populate_Fail) {
  vector<string> columns = {"category", "col2", "col3"};

  Table table("my_table");
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_THROW(table.Populate(nullptr), std::string);
}

/**
 * Check that when we have a column called "category" it's auto
 * detected by the Populate method and the categories are validated
 * as being actual categories.
 *
 */
TEST(Parameters, Table_Populate_Categories) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();

  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "col2", "col3"};
  vector<string> row1 = { "male.immature.notag", "x", "x" };
  vector<string> row2 = { "female.immature.notag", "x", "x" };
  vector<string> row3 = { "male.mature.2001", "x", "x" };

  Table table("my_table");
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_NO_THROW(table.Populate(&model));
}

/**
 * Check that when we have a column called "category" it's auto
 * detected by the Populate method and the categories are validated
 * as being actual categories.
 *
 * This method will detect the invalid category "male" and throw an exception
 */
TEST(Parameters, Table_Populate_Categories_Fail) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "col2", "col3"};
  vector<string> row1 = { "male.immature.notag", "x", "x" };
  vector<string> row2 = { "female.immature.notag", "x", "x" };
  vector<string> row3 = { "male", "x", "x" };

  Table table("my_table");
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_THROW(table.Populate(&model), std::string);
}

/**
 * Check that when we have a column called "category" it's auto
 * detected by the Populate method and the categories are validated
 * as being actual categories.
 *
 * This method will use shorthand syntax for the category entry to define
 * multiple categories. The table will be split up to duplicate each row
 * so the final table will have more rows with 1 category per row.
 */
TEST(Parameters, Table_Populate_Shorthand_Categories) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "col2", "col3"};
  vector<string> row1 = { "male.immature.notag", "x", "x" };
  vector<string> row2 = { "female.immature.notag", "x", "x" };
  vector<string> row3 = { "male,female.mature.2001:2002", "y", "z" };

  Table table("my_table");
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_NO_THROW(table.Populate(&model));
  EXPECT_EQ(6u, table.row_count());
}

/**
 * Check that when we have a column called "category" it's auto
 * detected by the Populate method and the categories are validated
 * as being actual categories.
 *
 * This method will use shorthand syntax for the category entry to define
 * multiple categories. The table will be split up to duplicate each row
 * so the final table will have more rows with 1 category per row.
 *
 * This test is checking for the short hand syntax being expanded correctly
 * then failing on the category test for male.mature.female as this is
 * not a valid category
 */
TEST(Parameters, Table_Populate_Shorthand_Categories_Fail) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "col2", "col3"};
  vector<string> row1 = { "male.immature.notag", "x", "x" };
  vector<string> row2 = { "male,female.mature.2001:2002", "x", "x" };
  vector<string> row3 = { "male.mature.2001,female", "y", "z" };

  Table table("my_table");
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_THROW(table.Populate(&model), std::string);
}

/**
 * This will test that when we define required columns on a table the columns
 * provided are checked and passes.
 */
TEST(Parameters, Table_Required_Columns) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "col2", "col3"};
  vector<string> required_columns = {"category", "col2", "col3"};
  vector<string> row1 = { "male.immature.notag", "x", "x" };
  vector<string> row2 = { "male,female.mature.2001:2002", "x", "x" };
  vector<string> row3 = { "male.mature.2001", "y", "z" };

  Table table("my_table");
  EXPECT_NO_THROW(table.set_required_columns(required_columns, false));
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_NO_THROW(table.Populate(&model));
}

/**
 * This will test that when we define required columns on a table the columns
 * provided are checked.
 *
 * This test is checking for a failure because col2 and col3 are not provided as
 * columns in the table's header row
 */
TEST(Parameters, Table_Required_Columns_Fail) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = { "category" };
  vector<string> required_columns = { "category", "col2", "col3" };
  vector<string> row1 = { "male.immature.notag", "x", "x" };
  vector<string> row2 = { "male,female.mature.2001:2002", "x", "x" };
  vector<string> row3 = { "male.mature.2001", "y", "z" };

  Table table("my_table");
  EXPECT_NO_THROW(table.set_required_columns(required_columns, false));
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_THROW(table.Populate(&model), std::string);
}

/**
 * This will test that the table accepts extra columns when we have
 * defined required columns with the allow_others flag set to true.
 *
 */
TEST(Parameters, Table_Extra_Columns) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "col2", "col3"};
  vector<string> required_columns = {"category", "col2", "col3", "col4", "col5"};
  vector<string> row1 = { "male.immature.notag", "x", "x" };
  vector<string> row2 = { "male,female.mature.2001:2002", "x", "x" };
  vector<string> row3 = { "male.mature.2001", "y", "z" };

  Table table("my_table");
  EXPECT_NO_THROW(table.set_required_columns(required_columns, true));
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_THROW(table.Populate(&model), std::string);
}

/**
 * This will test that the model accepts required columns and optional columns
 * but not flagging allow_others. This allows us to have a table with a fixed
 * set of columns to be provided plus some optionals depending on circumstances.
 *
 */
TEST(Parameters, Table_Optional_Columns) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "year", "values"};
  vector<string> required_columns = {"category", "year"};
  vector<string> optional_columns = { "values" };
  vector<string> row1 = { "male.immature.notag", "1990", "x" };
  vector<string> row2 = { "male,female.mature.2001:2002", "1990", "x" };
  vector<string> row3 = { "male.mature.2001", "1991", "z" };

  Table table("my_table");
  EXPECT_NO_THROW(table.set_required_columns(required_columns, false));
  EXPECT_NO_THROW(table.set_optional_columns(optional_columns));
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_NO_THROW(table.Populate(&model));
}

/**
 * This will test that the model accepts required columns and optional columns
 * but not flagging allow_others. This allows us to have a table with a fixed
 * set of columns to be provided plus some optionals depending on circumstances.
 *
 * This test provides a column to the table that is not in the optional columns
 * or required columns list and we have allow_others as False.
 */
TEST(Parameters, Table_Optional_Columns_Fail) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "year", "values"};
  vector<string> required_columns = {"category", "year"};
  vector<string> optional_columns = { "labels" };
  vector<string> row1 = { "male.immature.notag", "x", "x" };
  vector<string> row2 = { "male,female.mature.2001:2002", "x", "x" };
  vector<string> row3 = { "male.mature.2001", "y", "z" };

  Table table("my_table");
  EXPECT_NO_THROW(table.set_required_columns(required_columns, false));
  EXPECT_NO_THROW(table.set_optional_columns(optional_columns));
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_THROW(table.Populate(&model), std::string);
}

/**
 * Check adding some valid categories will work and they'll be detected.
 */
TEST(Parameters, Table_CheckColumnType_Unsigned_And_Double) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "year", "value" };
  vector<string> row1 = { "male.immature.notag", "1990", "1.0" };
  vector<string> row2 = { "female.mature.2001",  "1991", "2.5" };
  vector<string> row3 = { "male.mature.2001",    "1992", "3.9" };

  Table table("my_table");
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_NO_THROW(table.Populate(&model));

  EXPECT_NO_THROW(table.CheckColumnValuesAreType<unsigned>("year"));
  EXPECT_NO_THROW(table.CheckColumnValuesAreType<double>("value"));
}

/**
 * Check adding some valid categories will work and they'll be detected.
 */
TEST(Parameters, Table_CheckColumnType_Unsigned_Fail) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "yearx", "value" }; // Cannot use year as it's reserved
  vector<string> row1 = { "male.immature.notag", "1990", "1.0" };
  vector<string> row2 = { "female.mature.2001",  "hello", "2.5" };
  vector<string> row3 = { "male.mature.2001",    "1992", "3.9" };

  Table table("my_table");
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_NO_THROW(table.Populate(&model));

  EXPECT_THROW(table.CheckColumnValuesAreType<unsigned>("yearx"), std::string);
  EXPECT_NO_THROW(table.CheckColumnValuesAreType<double>("value"));
}

/**
 * Check adding some valid categories will work and they'll be detected.
 */
TEST(Parameters, Table_CheckColumnType_Double_Fail) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "year", "value" };
  vector<string> row1 = { "male.immature.notag", "1990", "1.0" };
  vector<string> row2 = { "female.mature.2001",  "1991", "2.5" };
  vector<string> row3 = { "male.mature.2001",    "1992", "lemon" };

  Table table("my_table");
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_NO_THROW(table.Populate(&model));

  EXPECT_NO_THROW(table.CheckColumnValuesAreType<unsigned>("year"));
  EXPECT_THROW(table.CheckColumnValuesAreType<double>("value"), std::string);
}

/**
 * Check adding some valid categories will work and they'll be detected.
 */
TEST(Parameters, Table_CheckColumnContains_Categories) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "year", "value" };
  vector<string> row1 = { "male.immature.notag", "1990", "1.0" };
  vector<string> row2 = { "female.mature.2001",  "1991", "2.5" };
  vector<string> row3 = { "male.mature.2001",    "1992", "3.0" };

  Table table("my_table");
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_NO_THROW(table.Populate(&model));

  vector<string> category_labels = categories.category_names();
  EXPECT_NO_THROW(table.CheckColumnValuesContain<string>("category", category_labels));
}

/**
 * Check adding some valid categories will work and they'll be detected.
 */
TEST(Parameters, Table_CheckColumnContains_Year) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "year", "value" };
  vector<string> row1 = { "male.immature.notag", "1990", "1.0" };
  vector<string> row2 = { "female.mature.2001",  "1991", "2.5" };
  vector<string> row3 = { "male.mature.2001",    "1992", "3.0" };
  vector<unsigned> years = { 1990, 1991, 1992, 1993, 1994, 1995 };

  Table table("my_table");
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_NO_THROW(table.Populate(&model));

  vector<string> category_labels = categories.category_names();
  EXPECT_NO_THROW(table.CheckColumnValuesContain<unsigned>("year", years));
}

/**
 * Check adding some valid categories will work and they'll be detected.
 */
TEST(Parameters, Table_CheckColumnContains_Projection_Year) {
  MockModel model;
  MockCategories categories(&model);

  model.set_projection_final_year(1995);
  model.bind_calls();

  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "year", "value" };
  vector<string> row1 = { "male.immature.notag", "1990", "1.0" };
  vector<string> row2 = { "female.mature.2001",  "1991", "2.5" };
  vector<string> row3 = { "male.mature.2001",    "1994", "3.0" };
  vector<unsigned> years = { 1990, 1991, 1992, 1993, 1994, 1995 };

  Table table("my_table");
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_NO_THROW(table.Populate(&model));

  vector<string> category_labels = categories.category_names();
  EXPECT_NO_THROW(table.CheckColumnValuesContain<unsigned>("year", years));
}

/**
 * Check adding some valid categories will work and they'll be detected.
 */
TEST(Parameters, Table_CheckColumnContains_Year_Fail) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "yearx", "value" }; // cannot use year cause it's auto-detected
  vector<string> row1 = { "male.immature.notag", "1990", "1.0" };
  vector<string> row2 = { "female.mature.2001",  "1991", "2.5" };
  vector<string> row3 = { "male.mature.2001",    "1999", "3.0" };
  vector<unsigned> years = { 1990, 1991, 1992, 1993, 1994, 1995 };

  Table table("my_table");
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_NO_THROW(table.Populate(&model));

  vector<string> category_labels = categories.category_names();
  EXPECT_THROW(table.CheckColumnValuesContain<unsigned>("yearx", years), std::string);
}

/**
 * Check adding some valid categories will work and they'll be detected.
 */
TEST(Parameters, Table_CheckColumnContains_Projection_Year_Fail) {
  MockModel model;
  MockCategories categories(&model);

  model.set_projection_final_year(1995);
  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "yearx", "value" }; // cannot use year cause it's auto-detected
  vector<string> row1 = { "male.immature.notag", "1990", "1.0" };
  vector<string> row2 = { "female.mature.2001",  "1991", "2.5" };
  vector<string> row3 = { "male.mature.2001",    "1999", "3.0" };
  vector<unsigned> years = { 1990, 1991, 1992, 1993, 1994, 1995 };

  Table table("my_table");
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_NO_THROW(table.Populate(&model));

  vector<string> category_labels = categories.category_names();
  EXPECT_THROW(table.CheckColumnValuesContain<unsigned>("yearx", years), std::string);
}

/**
 * Check adding some valid categories will work and they'll be detected.
 */
TEST(Parameters, Table_GetColumnValuesAs_Unsigned_Double) {
  MockModel model;
  MockCategories categories(&model);

  model.bind_calls();
  ASSERT_NO_THROW(categories.Validate());

  EXPECT_CALL(model, categories()).WillRepeatedly(Return(&categories));

  vector<string> columns = {"category", "year", "value" };
  vector<string> row1 = { "male.immature.notag", "1990", "1.0" };
  vector<string> row2 = { "female.mature.2001",  "1991", "2.5" };
  vector<string> row3 = { "male.mature.2001",    "1992", "3.0" };
  vector<unsigned> years = { 1990, 1991, 1992, 1993, 1994, 1995 };

  Table table("my_table");
  EXPECT_NO_THROW(table.AddColumns(columns));
  EXPECT_NO_THROW(table.AddRow(row1));
  EXPECT_NO_THROW(table.AddRow(row2));
  EXPECT_NO_THROW(table.AddRow(row3));

  EXPECT_NO_THROW(table.Populate(&model));

  vector<string> category_labels = categories.category_names();
  vector<unsigned> x;
  EXPECT_NO_THROW(x = table.GetColumnValuesAs<unsigned>("year"));
  EXPECT_EQ(3u, x.size());

  vector<double> y;
  EXPECT_NO_THROW(y = table.GetColumnValuesAs<double>("value"));
  EXPECT_EQ(3u, y.size());

  EXPECT_NO_THROW(vector<string> z = table.GetColumnValuesAs<string>("category"));
}

} /* namespace parameters */
} /* namespace niwa */


#endif /* TESTMODE */
