/**
 * @file Model.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/01/2019
 * @section LICENSE
 *
 * Copyright NIWA Science ©2018 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include "Model.h"

#include <iostream>

#include "TestResources/TestFixtures/BasicModel.h"
#include "TestResources/MockClasses/Managers.h"
#include "TestResources/MockClasses/Model.h"

// Namespaces
namespace niwa {

using std::cout;
using std::endl;
using ::testing::NiceMock;

/**
 * See if we can create a Model object and check the default values
 */
TEST(Model, Validate_Defaults) {
  Model model;

  EXPECT_EQ(RunMode::kInvalid, model.run_mode());
  EXPECT_EQ(State::kStartUp, model.state());
  EXPECT_EQ(0u, model.start_year());
  EXPECT_EQ(0u, model.final_year());
  EXPECT_EQ(0u, model.projection_final_year());
  EXPECT_EQ(0u, model.current_year());
  EXPECT_EQ(0u, model.min_age());
  EXPECT_EQ(0u, model.max_age());
  EXPECT_EQ("", model.base_weight_units());
  EXPECT_EQ(true, model.age_plus());
  EXPECT_EQ(0u, model.initialisation_phases().size());
  EXPECT_EQ(0u, model.time_steps().size());
  EXPECT_EQ(true, model.length_plus());
  EXPECT_EQ(0u, model.length_plus_group());
  EXPECT_EQ(PartitionType::kInvalid, model.partition_type());
  EXPECT_NE(nullptr, &model.managers());
  EXPECT_NE(nullptr, &model.objects());
  EXPECT_NE(nullptr, &model.global_configuration());
  EXPECT_NE(nullptr, model.categories());
  EXPECT_NE(nullptr, &model.factory());
}

/**
 * Create a NiceMock. This will just throw a bunch of default
 * values in for things. Defaulting to 0 or false respectively.
 *
 * Do not advise using NickMocks.
 */
TEST(Model, Mock_Validate_Defaults_With_NiceMock) {
  NiceMock<MockModel> model;

  EXPECT_EQ(RunMode::kInvalid, model.run_mode());
  EXPECT_EQ(State::kStartUp, model.state());
  EXPECT_EQ(0u, model.start_year());
  EXPECT_EQ(0u, model.final_year());
  EXPECT_EQ(0u, model.projection_final_year());
  EXPECT_EQ(0u, model.current_year());
  EXPECT_EQ(0u, model.min_age());
  EXPECT_EQ(0u, model.max_age());
  EXPECT_EQ("", model.base_weight_units());
  EXPECT_EQ(0u, model.length_plus_group());
  EXPECT_EQ(false, model.age_plus());
  EXPECT_EQ(false, model.length_plus());
}

/**
 * The .DoDefault basically works the same as a NiceMock object. It will ignore
 * the values assigned to the object and use the default values
 * of 0 and false etc.
 */
TEST(Model, Validate_Mock_With_DoDefault) {
  MockModel model;
  model.bind_calls_to_default();

  EXPECT_EQ(RunMode::kInvalid, model.run_mode());
  EXPECT_EQ(State::kStartUp, model.state());
  EXPECT_EQ(0u, model.start_year());
  EXPECT_EQ(0u, model.final_year());
  EXPECT_EQ(0u, model.projection_final_year());
  EXPECT_EQ(0u, model.current_year());
  EXPECT_EQ(0u, model.min_age());
  EXPECT_EQ(0u, model.max_age());
  EXPECT_EQ("", model.base_weight_units());
  EXPECT_EQ(0u, model.length_plus_group());
  EXPECT_EQ(false, model.age_plus());
  EXPECT_EQ(false, model.length_plus());
}

/**
 * The .DoDefault basically works the same as a NiceMock object. It will ignore
 * the values assigned to the object and use the default values
 * of 0 and false etc.
 */
TEST(Model, Validate_Mock_With_Init_01) {
  MockModel model;
  model.bind_calls();

  EXPECT_EQ(RunMode::kInvalid, model.run_mode());
  EXPECT_EQ(State::kStartUp, model.state());
  EXPECT_EQ(1990u, model.start_year());
  EXPECT_EQ(1992u, model.final_year());
  EXPECT_EQ(0u, model.projection_final_year());
  EXPECT_EQ(1991u, model.current_year());
  EXPECT_EQ(1u, model.min_age());
  EXPECT_EQ(10u, model.max_age());
  EXPECT_EQ("", model.base_weight_units());
  EXPECT_EQ(0u, model.length_plus_group());
  EXPECT_EQ(true, model.age_plus());
  EXPECT_EQ(false, model.length_plus());
  EXPECT_EQ(0u, model.initialisation_phases().size());
  EXPECT_EQ(2u, model.time_steps().size());
  EXPECT_EQ(PartitionType::kAge, model.partition_type());
}

} /* namespace niwa */


#endif /* TESTMODE */
