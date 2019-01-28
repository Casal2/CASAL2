/**
 * @file Objects.Test.cpp
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
#include "Objects.h"
#include "Objects.Mock.h"

#include <iostream>
#include <string>

#include "Selectivities/Common/Constant.Mock.h"
#include "TestResources/MockClasses/Model.h"

// Namespaces
namespace niwa {

using std::cout;
using std::endl;
using ::testing::_;

/**
 * See if we can create a Model object and check the default values
 */
TEST(Objects, Validate_Mock) {
  MockModel model;
  MockObjects objects(&model);
  std::string error = "";
  double double_ = 10.0;
  double double2_ = 20.0;

  EXPECT_CALL(objects, VerfiyAddressableForUse(_, _, _)).WillRepeatedly(Return(true));
  EXPECT_EQ(true, objects.VerfiyAddressableForUse("", addressable::kTimeVarying, error));

  EXPECT_CALL(objects, VerfiyAddressableForUse(_, _, _)).WillRepeatedly(Return(false));
  EXPECT_EQ(false, objects.VerfiyAddressableForUse("", addressable::kTimeVarying, error));

  EXPECT_CALL(objects, GetAddressable(_)).WillRepeatedly(Return(&double_));
  EXPECT_EQ(&double_, objects.GetAddressable(""));
  EXPECT_EQ(double_, *objects.GetAddressable(""));
  EXPECT_DOUBLE_EQ(10.0, *objects.GetAddressable(""));

  vector<double*> addressables_ = { &double_, &double2_ };
  EXPECT_CALL(objects, GetAddressables(_)).WillRepeatedly(Return(&addressables_));
  ASSERT_EQ(&addressables_, objects.GetAddressables(""));
  ASSERT_EQ(2u, objects.GetAddressables("")->size());
  EXPECT_DOUBLE_EQ(double_, *(*objects.GetAddressables(""))[0]);
  EXPECT_DOUBLE_EQ(double2_, *(*objects.GetAddressables(""))[1]);

  std::map<unsigned, double> umap_;
  umap_[1990] = 1.0;
  umap_[1991] = 1.5;
  EXPECT_CALL(objects, GetAddressableUMap(_)).WillRepeatedly(Return(&umap_));
  ASSERT_EQ(&umap_, objects.GetAddressableUMap(""));
  EXPECT_EQ(2u, (*objects.GetAddressableUMap("")).size());
  EXPECT_DOUBLE_EQ(1.0, (*objects.GetAddressableUMap(""))[1990]);
  EXPECT_DOUBLE_EQ(1.5, (*objects.GetAddressableUMap(""))[1991]);

  OrderedMap<string, double> ordered_map_;
  ordered_map_["apple"]  = 1.0;
  ordered_map_["carrot"] = 2.0;
  ordered_map_["banana"] = 3.0;
  EXPECT_CALL(objects, GetAddressableSMap(_)).WillRepeatedly(Return(&ordered_map_));
  ASSERT_EQ(&ordered_map_, objects.GetAddressableSMap(""));
  EXPECT_EQ(3u, objects.GetAddressableSMap("")->size());

  std::vector<double> vector_ = { 1.0, 2.0, 3.0 };
  EXPECT_CALL(objects, GetAddressableVector(_)).WillRepeatedly(Return(&vector_));
  ASSERT_EQ(&vector_, objects.GetAddressableVector(""));
  EXPECT_EQ(3u, objects.GetAddressableVector("")->size());

  selectivities::MockConstant c(&model);
  EXPECT_CALL(objects, FindObject(_)).WillRepeatedly(Return(&c));
  ASSERT_EQ(&c, objects.FindObject(""));
}

//MOCK_METHOD1(FindObject, base::Object*(const string&));

/**
 * Create a NiceMock. This will just throw a bunch of default
 * values in for things. Defaulting to 0 or false respectively.
 *
 * Do not advise using NickMocks.
 */
TEST(Objects, Mock_Validate_Defaults_With_NiceMock) {
//  NiceMock<MockModel> model;
//
//  EXPECT_EQ(RunMode::kInvalid, model.run_mode());
//  EXPECT_EQ(State::kStartUp, model.state());
//  EXPECT_EQ(0u, model.start_year());
//  EXPECT_EQ(0u, model.final_year());
//  EXPECT_EQ(0u, model.projection_final_year());
//  EXPECT_EQ(0u, model.current_year());
//  EXPECT_EQ(0u, model.min_age());
//  EXPECT_EQ(0u, model.max_age());
//  EXPECT_EQ("", model.base_weight_units());
//  EXPECT_EQ(0u, model.length_plus_group());
//  EXPECT_EQ(false, model.age_plus());
//  EXPECT_EQ(false, model.length_plus());
}

/**
 * The .DoDefault basically works the same as a NiceMock object. It will ignore
 * the values assigned to the object and use the default values
 * of 0 and false etc.
 */
TEST(Objects, Validate_Mock_With_DoDefault) {
//  MockModel model;
//  model.bind_calls_to_default();
//
//  EXPECT_EQ(RunMode::kInvalid, model.run_mode());
//  EXPECT_EQ(State::kStartUp, model.state());
//  EXPECT_EQ(0u, model.start_year());
//  EXPECT_EQ(0u, model.final_year());
//  EXPECT_EQ(0u, model.projection_final_year());
//  EXPECT_EQ(0u, model.current_year());
//  EXPECT_EQ(0u, model.min_age());
//  EXPECT_EQ(0u, model.max_age());
//  EXPECT_EQ("", model.base_weight_units());
//  EXPECT_EQ(0u, model.length_plus_group());
//  EXPECT_EQ(false, model.age_plus());
//  EXPECT_EQ(false, model.length_plus());
}

/**
 * The .DoDefault basically works the same as a NiceMock object. It will ignore
 * the values assigned to the object and use the default values
 * of 0 and false etc.
 */
TEST(Objects, Validate_Mock_With_Init_01) {
//  MockModel model;
//  model.bind_calls();
//
//  EXPECT_EQ(RunMode::kInvalid, model.run_mode());
//  EXPECT_EQ(State::kStartUp, model.state());
//  EXPECT_EQ(1990u, model.start_year());
//  EXPECT_EQ(1992u, model.final_year());
//  EXPECT_EQ(0u, model.projection_final_year());
//  EXPECT_EQ(1991u, model.current_year());
//  EXPECT_EQ(1u, model.min_age());
//  EXPECT_EQ(10u, model.max_age());
//  EXPECT_EQ("", model.base_weight_units());
//  EXPECT_EQ(0u, model.length_plus_group());
//  EXPECT_EQ(true, model.age_plus());
//  EXPECT_EQ(false, model.length_plus());
//  EXPECT_EQ(0u, model.initialisation_phases().size());
//  EXPECT_EQ(2u, model.time_steps().size());
//  EXPECT_EQ(PartitionType::kAge, model.partition_type());
}

} /* namespace niwa */


#endif /* TESTMODE */
