/**
 * @file model->Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/01/2019
 * @section LICENSE
 *
 * Copyright NIWA Science �2018 - www.niwa.co.nz
 *
 */
#ifdef TESTMODE

// Headers
#include <iostream>

#include "../TestResources/MockClasses/Managers.h"
#include "../TestResources/MockClasses/Model.h"
#include "../TestResources/TestFixtures/BasicModel.h"
#include "Model.h"

// Namespaces
namespace niwa {

using std::cout;
using std::endl;
using ::testing::NiceMock;

/**
 * See if we can create a Model object and check the default values
 */
TEST(Model, Validate_Defaults) {
  shared_ptr<Model> model = shared_ptr<Model>(new Model());

  EXPECT_EQ(RunMode::kInvalid, model->run_mode());
  EXPECT_EQ(State::kStartUp, model->state());
  EXPECT_EQ(0u, model->start_year());
  EXPECT_EQ(0u, model->final_year());
  EXPECT_EQ(0u, model->projection_final_year());
  EXPECT_EQ(0u, model->current_year());
  EXPECT_EQ(0u, model->min_age());
  EXPECT_EQ(0u, model->max_age());
  EXPECT_EQ("", model->base_weight_units());
  EXPECT_EQ(true, model->age_plus());
  EXPECT_EQ(0u, model->initialisation_phases().size());
  EXPECT_EQ(0u, model->time_steps().size());
  EXPECT_EQ(true, model->length_plus());
  EXPECT_EQ(0u, model->length_plus_group());
  EXPECT_EQ(PartitionType::kInvalid, model->partition_type());
  //  EXPECT_NE(nullptr, model->managers());
  //  EXPECT_NE(nullptr, model->objects());
  //  EXPECT_NE(nullptr, model->global_configuration());
  //  EXPECT_NE(nullptr, model->categories());
  //  EXPECT_NE(nullptr, model->factory());
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
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  model->bind_calls_to_default();

  EXPECT_EQ(RunMode::kInvalid, model->run_mode());
  EXPECT_EQ(State::kStartUp, model->state());
  EXPECT_EQ(0u, model->start_year());
  EXPECT_EQ(0u, model->final_year());
  EXPECT_EQ(0u, model->projection_final_year());
  EXPECT_EQ(0u, model->current_year());
  EXPECT_EQ(0u, model->min_age());
  EXPECT_EQ(0u, model->max_age());
  EXPECT_EQ("", model->base_weight_units());
  EXPECT_EQ(0u, model->length_plus_group());
  EXPECT_EQ(false, model->age_plus());
  EXPECT_EQ(false, model->length_plus());
}

/**
 * The .DoDefault basically works the same as a NiceMock object. It will ignore
 * the values assigned to the object and use the default values
 * of 0 and false etc.
 */
TEST(Model, Validate_are_length_bin_compatible_with_model_length_bins) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  model->set_length_plus(true);
  model->set_length_bins({0, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47});
  model->set_number_of_length_bins(); // if we chnage plus group need to reset thsi
  model->bind_calls();
  //
  vector<double> correct_subset = {15, 20, 30, 47};
  vector<double> incorrect_subset = {6, 20, 30, 47};
  vector<double> incorrect_subset2 = {15, 20, 30, 50};

  EXPECT_TRUE(model->are_length_bin_compatible_with_model_length_bins(correct_subset));
  EXPECT_FALSE(model->are_length_bin_compatible_with_model_length_bins(incorrect_subset));
  EXPECT_FALSE(model->are_length_bin_compatible_with_model_length_bins(incorrect_subset2));
}

/**
 * Validate methods for bespoke length bin class
 */
TEST(Model, Validate_get_map_for_bespoke_length_bins_to_global_length_bins) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  model->set_length_plus(true);
  model->set_length_bins({0, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47});
  model->set_number_of_length_bins(); // if we chnage plus group need to reset thsi
  model->bind_calls();
  //Case 1
  vector<int> expected_result = {-9999, 0, 0 ,0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3};
  vector<double> length_subset_1 = {15, 20, 30, 47};
  bool plus_group = true;
  vector<int> case1_result = model->get_map_for_bespoke_length_bins_to_global_length_bins(length_subset_1, plus_group);
  EXPECT_EQ(case1_result.size(), expected_result.size());
  for(unsigned i = 0; i < case1_result.size(); ++i)
    EXPECT_EQ(expected_result[i], case1_result[i]) << " element : " << i + 1 << std::endl;


  //Case 2
  length_subset_1 = {15, 20, 30, 40};
  vector<int> expect_result_2 = {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999};
  plus_group = false;
  vector<int> case2_result = model->get_map_for_bespoke_length_bins_to_global_length_bins(length_subset_1, plus_group);
  EXPECT_EQ(expected_result.size(), case2_result.size());
  for(unsigned i = 0; i < case2_result.size(); ++i)
    EXPECT_EQ(case2_result[i], case2_result[i]) << " element : " << i + 1 << std::endl;


}


/**
 * Validate get model length bin nex
 */
TEST(Model, Validate_get_length_bin_ndx) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  model->set_length_plus(true);
  model->set_length_bins({0, 15, 20, 25, 30, 50});
  model->set_number_of_length_bins(); // if we chnage plus group need to reset thsi
  model->bind_calls();
  //Case 1
  EXPECT_EQ(model->get_length_bin_ndx(10.0), 0);
  EXPECT_EQ(model->get_length_bin_ndx(15.0), 1); // tied value to length bin
  EXPECT_EQ(model->get_length_bin_ndx(17.0), 1);
  EXPECT_EQ(model->get_length_bin_ndx(21.0), 2);
  EXPECT_EQ(model->get_length_bin_ndx(27.0), 3);
  EXPECT_EQ(model->get_length_bin_ndx(32.0), 4);
  EXPECT_EQ(model->get_length_bin_ndx(52.0), 5);
}
} /* namespace niwa */

#endif /* TESTMODE */
