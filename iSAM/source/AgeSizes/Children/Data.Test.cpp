/**
 * @file Data.Test.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 3/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "AgeSizes/Manager.h"
#include "ObjectiveFunction/ObjectiveFunction.h"
#include "Estimates/Manager.h"
#include "Model/Model.h"
#include "TestResources/TestFixtures/InternalEmptyModel.h"
#include "TestResources/Models/TwoSexHalfAges.h"

// Namespaces
namespace isam {
namespace estimates {

using std::cout;
using std::endl;
using isam::testfixtures::InternalEmptyModel;

const string age_size_data_external_mean_internal_mean =
R"(
@age_size test_age_size
type data
size_weight [type=none]
external_gaps mean
internal_gaps mean
table data
year 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
1977 25.31 30.70 34.36 36.92 37.76 38.85 38.51 38.07 37.57 38.06 37.91 37.71 37.85 38.02 39 45 41 47 43 44 45 46 46 47
1990 30.31 35.70 39.36 41.92 42.76 43.85 43.51 43.07 42.57 43.06 42.91 42.71 42.85 43.02 44 45 46 47 48 49 50 51 51 52
2000 35.31 40.70 44.36 46.92 47.76 48.85 48.51 48.07 47.57 48.06 47.91 47.71 47.85 48.02 49 45 51 47 53 54 55 56 56 57
end_table
)";

/**
 *
 */
TEST_F(InternalEmptyModel, AgeSizes_Data_Mean_Mean) {
  AddConfigurationLine(testresources::models::two_sex_half_ages, "TestResources/Models/TwoSexHalfAges.h", 24);
  AddConfigurationLine(age_size_data_external_mean_internal_mean, __FILE__, 33);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = ObjectiveFunction::Instance();
  EXPECT_DOUBLE_EQ(16884.277966840153, obj_function.score());

  AgeSizePtr age_size = agesizes::Manager::Instance().GetAgeSize("test_age_size");
  if (!age_size)
    LOG_ERROR("!age_size");

  // Check results
  EXPECT_DOUBLE_EQ(1.0, age_size->mean_weight(2000, 2));
  EXPECT_DOUBLE_EQ(35.31, age_size->mean_size(2000, 2));

  EXPECT_DOUBLE_EQ(1.0, age_size->mean_weight(2001, 2));
  EXPECT_DOUBLE_EQ(30.31, age_size->mean_size(2001, 2));

  EXPECT_DOUBLE_EQ(1.0, age_size->mean_weight(1996, 2));
  EXPECT_DOUBLE_EQ(30.31, age_size->mean_size(1996, 2));
}

/**
 *
 */
const string age_size_data_external_nearest_neighbour_internal_mean =
R"(
@age_size test_age_size
type data
size_weight [type=none]
external_gaps nearest_neighbour
internal_gaps mean
table data
year 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
1977 25.31 30.70 34.36 36.92 37.76 38.85 38.51 38.07 37.57 38.06 37.91 37.71 37.85 38.02 39 45 41 47 43 44 45 46 46 47
1990 30.31 35.70 39.36 41.92 42.76 43.85 43.51 43.07 42.57 43.06 42.91 42.71 42.85 43.02 44 45 46 47 48 49 50 51 51 52
2000 35.31 40.70 44.36 46.92 47.76 48.85 48.51 48.07 47.57 48.06 47.91 47.71 47.85 48.02 49 45 51 47 53 54 55 56 56 57
end_table
)";

/**
 *
 */
TEST_F(InternalEmptyModel, AgeSizes_Data_NearestNeighbour_Mean) {
  AddConfigurationLine(testresources::models::two_sex_half_ages, "TestResources/Models/TwoSexHalfAges.h", 24);
  AddConfigurationLine(age_size_data_external_nearest_neighbour_internal_mean, __FILE__, 80);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = ObjectiveFunction::Instance();
  EXPECT_DOUBLE_EQ(16884.277966840153, obj_function.score());

  AgeSizePtr age_size = agesizes::Manager::Instance().GetAgeSize("test_age_size");
  if (!age_size)
    LOG_ERROR("!age_size");

  // Check results
  EXPECT_DOUBLE_EQ(1.0, age_size->mean_weight(1976, 2));
  EXPECT_DOUBLE_EQ(25.31, age_size->mean_size(1976, 2));

  EXPECT_DOUBLE_EQ(1.0, age_size->mean_weight(1996, 2));
  EXPECT_DOUBLE_EQ(30.31, age_size->mean_size(1996, 2));

  EXPECT_DOUBLE_EQ(1.0, age_size->mean_weight(2000, 2));
  EXPECT_DOUBLE_EQ(35.31, age_size->mean_size(2000, 2));

  EXPECT_DOUBLE_EQ(1.0, age_size->mean_weight(2001, 2));
  EXPECT_DOUBLE_EQ(35.31, age_size->mean_size(2001, 2));
}

/**
 *
 */
const string age_size_data_external_mean_internal_nearest_neighbour =
R"(
@age_size test_age_size
type data
size_weight [type=none]
external_gaps mean
internal_gaps nearest_neighbour
table data
year 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
1977 25.31 30.70 34.36 36.92 37.76 38.85 38.51 38.07 37.57 38.06 37.91 37.71 37.85 38.02 39 45 41 47 43 44 45 46 46 47
1990 30.31 35.70 39.36 41.92 42.76 43.85 43.51 43.07 42.57 43.06 42.91 42.71 42.85 43.02 44 45 46 47 48 49 50 51 51 52
2000 35.31 40.70 44.36 46.92 47.76 48.85 48.51 48.07 47.57 48.06 47.91 47.71 47.85 48.02 49 45 51 47 53 54 55 56 56 57
end_table
)";

/**
 *
 */
TEST_F(InternalEmptyModel, AgeSizes_Data_Mean_NearestNeighbour) {
  AddConfigurationLine(testresources::models::two_sex_half_ages, "TestResources/Models/TwoSexHalfAges.h", 24);
  AddConfigurationLine(age_size_data_external_mean_internal_nearest_neighbour, __FILE__, 130);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = ObjectiveFunction::Instance();
  EXPECT_DOUBLE_EQ(16884.277966840153, obj_function.score());

  AgeSizePtr age_size = agesizes::Manager::Instance().GetAgeSize("test_age_size");
  if (!age_size)
    LOG_ERROR("!age_size");

  // Check results
  EXPECT_DOUBLE_EQ(1.0, age_size->mean_weight(1976, 2));
  EXPECT_DOUBLE_EQ(30.31, age_size->mean_size(1976, 2));

  EXPECT_DOUBLE_EQ(1.0, age_size->mean_weight(1991, 2));
  EXPECT_DOUBLE_EQ(30.31, age_size->mean_size(1991, 2));

  EXPECT_DOUBLE_EQ(1.0, age_size->mean_weight(1996, 2));
  EXPECT_DOUBLE_EQ(35.31, age_size->mean_size(1996, 2));

  EXPECT_DOUBLE_EQ(1.0, age_size->mean_weight(2001, 2));
  EXPECT_DOUBLE_EQ(30.31, age_size->mean_size(2001, 2));
}

/**
 *
 */
const string age_size_data_external_mean_internal_interpolate =
R"(
@age_size test_age_size
type data
size_weight [type=none]
external_gaps mean
internal_gaps interpolate
table data
year 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
1977 25.31 30.70 34.36 36.92 37.76 38.85 38.51 38.07 37.57 38.06 37.91 37.71 37.85 38.02 39 45 41 47 43 44 45 46 46 47
1990 30.31 35.70 39.36 41.92 42.76 43.85 43.51 43.07 42.57 43.06 42.91 42.71 42.85 43.02 44 45 46 47 48 49 50 51 51 52
2000 35.31 40.70 44.36 46.92 47.76 48.85 48.51 48.07 47.57 48.06 47.91 47.71 47.85 48.02 49 45 51 47 53 54 55 56 56 57
end_table
)";

/**
 *
 */
TEST_F(InternalEmptyModel, AgeSizes_Data_Mean_Interpolate) {
  AddConfigurationLine(testresources::models::two_sex_half_ages, "TestResources/Models/TwoSexHalfAges.h", 24);
  AddConfigurationLine(age_size_data_external_mean_internal_interpolate, __FILE__, 180);
  LoadConfiguration();

  ModelPtr model = Model::Instance();
  model->Start(RunMode::kBasic);

  ObjectiveFunction& obj_function = ObjectiveFunction::Instance();
  EXPECT_DOUBLE_EQ(16884.277966840153, obj_function.score());

  AgeSizePtr age_size = agesizes::Manager::Instance().GetAgeSize("test_age_size");
  if (!age_size)
    LOG_ERROR("!age_size");

  // Check results
  EXPECT_DOUBLE_EQ(1.0, age_size->mean_weight(1976, 2));
  EXPECT_DOUBLE_EQ(30.31, age_size->mean_size(1976, 2));

  EXPECT_DOUBLE_EQ(1.0, age_size->mean_weight(1991, 2));
  EXPECT_DOUBLE_EQ(30.865555555555556, age_size->mean_size(1991, 2));

  EXPECT_DOUBLE_EQ(1.0, age_size->mean_weight(1996, 2));
  EXPECT_DOUBLE_EQ(33.643333333333331, age_size->mean_size(1996, 2));

  EXPECT_DOUBLE_EQ(1.0, age_size->mean_weight(2001, 2));
  EXPECT_DOUBLE_EQ(30.31, age_size->mean_size(2001, 2));
}


} /* namespace estimates */
} /* namespace isam */
#endif
