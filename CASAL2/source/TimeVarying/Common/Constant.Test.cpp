/**
 * @file Constant.Test.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/01/2019
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */
#ifdef TESTMODE

// Headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "../../BaseClasses/Object.h"
#include "../../Model/Model.h"
#include "../../Model/Objects.h"
#include "../../Selectivities/Common/Constant.h"
#include "../../TestResources/MockClasses/Model.h"
#include "Constant.Mock.h"
#include "Constant.h"

// Namespaces
namespace niwa {
namespace timevarying {

using std::cout;
using std::endl;
using ::testing::_;

class MockSelectivity : public selectivities::Constant {
public:
  MockSelectivity(shared_ptr<Model> model) : selectivities::Constant(model) {}
  virtual ~MockSelectivity() = default;
  MOCK_METHOD2(GetAgeResult, double(unsigned age, AgeLength* age_length));
};

class MockObjects : public niwa::Objects {
public:
  MockObjects(shared_ptr<Model> model) : niwa::Objects(model) {}
  virtual ~MockObjects() = default;
  MOCK_METHOD1(FindObject, base::Object*(const string& parameter_absolute_name));
};

/**
 * Verify our Mock objects work as expected
 */
TEST(TimeVarying, Constant_Check_Mock_Selectivity) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  MockObjects           mock_objects(model);
  MockSelectivity       c(model);

  EXPECT_CALL(*model, partition_type()).WillRepeatedly(Return(PartitionType::kAge));
  EXPECT_CALL(*model, objects()).WillRepeatedly(ReturnRef(mock_objects));
  EXPECT_CALL(mock_objects, FindObject(_)).WillRepeatedly(Return(&c));
  EXPECT_CALL(c, GetAgeResult(_, nullptr)).WillRepeatedly(Return(10.0));

  EXPECT_EQ(&mock_objects, &model->objects());
  EXPECT_EQ(&c, model->objects().FindObject("X"));

  //EXPECT_EQ(10.0, c.GetAgeResult(0.0, nullptr));
  //EXPECT_EQ(10.0, c.GetAgeResult(1.0, nullptr));
}

/**
 * We'll create a simple time varying constant object,
 * then grab an addressable and modify it, restore it
 * to see if it's working properly.
 */
TEST(TimeVarying, Constant_Validate) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  MockObjects           mock_objects(model);
  MockSelectivity       c(model);

  EXPECT_CALL(*model, objects()).WillRepeatedly(ReturnRef(mock_objects));
  EXPECT_CALL(mock_objects, FindObject(_)).WillRepeatedly(Return(&c));
  EXPECT_CALL(c, GetAgeResult(_, nullptr)).WillRepeatedly(Return(10.0));

  vector<string> values = {"1.0", "2.0"};
  vector<string> years  = {"1990", "1992"};

  timevarying::Constant tv(model);
  tv.parameters().Add(PARAM_VALUES, values, __FILE__, __LINE__);
  tv.parameters().Add(PARAM_LABEL, "C", __FILE__, __LINE__);
  tv.parameters().Add(PARAM_TYPE, PARAM_CONSTANT, __FILE__, __LINE__);
  tv.parameters().Add(PARAM_YEARS, years, __FILE__, __LINE__);
  tv.parameters().Add(PARAM_PARAMETER, "x", __FILE__, __LINE__);

  ASSERT_NO_THROW(tv.Validate());
}

/**
 * We'll create a simple time varying constant object,
 * then grab an addressable and modify it, restore it
 * to see if it's working properly.
 */
TEST(TimeVarying, Constant_Validate_Fails) {
  shared_ptr<MockModel> model = shared_ptr<MockModel>(new MockModel());
  MockObjects           mock_objects(model);
  MockSelectivity       c(model);

  EXPECT_CALL(*model, objects()).WillRepeatedly(ReturnRef(mock_objects));
  EXPECT_CALL(mock_objects, FindObject(_)).WillRepeatedly(Return(&c));
  EXPECT_CALL(c, GetAgeResult(_, nullptr)).WillRepeatedly(Return(10.0));

  vector<string> years = {"1990", "1992"};

  timevarying::Constant tv(model);
  tv.parameters().Add(PARAM_VALUES, {"1.0", "2.0", "3.0"}, __FILE__, __LINE__);
  tv.parameters().Add(PARAM_LABEL, "C", __FILE__, __LINE__);
  tv.parameters().Add(PARAM_TYPE, PARAM_CONSTANT, __FILE__, __LINE__);
  tv.parameters().Add(PARAM_YEARS, years, __FILE__, __LINE__);
  tv.parameters().Add(PARAM_PARAMETER, "x", __FILE__, __LINE__);

  ASSERT_THROW(tv.Validate(), std::string);
}

} /* namespace timevarying */
} /* namespace niwa */

#endif /* TESTMODE */
