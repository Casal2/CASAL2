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

#include "Model/Managers.h"
#include "Processes/Manager.h"
#include "Processes/Common/UnitTester.h"
#include "Selectivities/Common/Constant.Mock.h"
#include "TestResources/MockClasses/Model.h"

// Namespaces
namespace niwa {

using std::cout;
using std::endl;
using ::testing::_;
using ::testing::Invoke;

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

  selectivities::MockConstant constant_selectivity(&model);
  EXPECT_CALL(objects, FindObject(_)).WillRepeatedly(Return(&constant_selectivity));
  ASSERT_EQ(&constant_selectivity, objects.FindObject(""));

  EXPECT_CALL(objects, FindObjectOrNull(_)).WillRepeatedly(Return(&constant_selectivity));
  ASSERT_EQ(&constant_selectivity, objects.FindObjectOrNull(""));

  const string a = "this is a";
  string b, c, d, e = "";
  EXPECT_CALL(objects, ExplodeString(_, _, _, _, _)).WillRepeatedly(Invoke(
      [=](const string& a, string& b, string& c, string& d, string& e) {
        b = "this is b";
        c = "this is c";
        d = "this is d";
        e = "this is e";
  }));
  objects.ExplodeString(a, b, c, d, e);
  EXPECT_EQ("this is b", b);
  EXPECT_EQ("this is c", c);
  EXPECT_EQ("this is d", d);
  EXPECT_EQ("this is e", e);

  const string a1 = "a1", b1 = "b1", c1 = "c1", d1 = "d1";
  string e1 = "";
  EXPECT_CALL(objects, ImplodeString(_, _, _, _, _)).WillRepeatedly(Invoke(
     [=](const string& a, const string& b, const string& c, const string& d, string& e) {
    e = a + b + c + d;
  }));
  objects.ImplodeString(a1, b1, c1, d1, e1);
  EXPECT_EQ("a1b1c1d1", e1);

  std::pair<string, string> ret;
  EXPECT_CALL(objects, ExplodeParameterAndIndex(_)).WillRepeatedly(Invoke(
     [=](const string& a) {
    return std::pair<string, string>("f", "s");
  }));

  ret = objects.ExplodeParameterAndIndex("");
  EXPECT_EQ("f", ret.first);
  EXPECT_EQ("s", ret.second);
}

/**
 * virtual bool VerfiyAddressableForUse(const string& parameter_absolute_name, addressable::Usage usage, string& error);
 */
TEST(Objects, VerifyAddressableForUse) {
  Model model;
  model.set_partition_type(PartitionType::kAge);
  Objects& objects = model.objects();
  ASSERT_NE(nullptr, &objects);

  string error = "";
  string parameter = "";

  EXPECT_EQ(false, objects.VerfiyAddressableForUse("bad parameter", addressable::kSimulate, error));
  ASSERT_EQ(true, (error.length() > 6));
  EXPECT_EQ("Syntax", error.substr(0, 6));

  EXPECT_EQ(false, objects.VerfiyAddressableForUse("process[recruitment].b0", addressable::kSimulate, error));
  ASSERT_EQ(true, (error.length() > 13));
  EXPECT_EQ("Parent object", error.substr(0, 13));

  // Now, we need to create an object and try to reference a non-existent addressable
  processes::UnitTester* unit_tester = new processes::UnitTester(&model);
  model.managers().process()->AddObject(unit_tester);
  unit_tester->parameters().Add(PARAM_LABEL, "unit_tester", __FILE__, __LINE__);
  unit_tester->parameters().Add(PARAM_TYPE, PARAM_NOP, __FILE__, __LINE__);
  model.managers().process()->Validate(&model);

  EXPECT_EQ(false, objects.VerfiyAddressableForUse("process[unit_tester].y", addressable::kSimulate, error));
  ASSERT_EQ(true, (error.length() > 16));
  EXPECT_EQ("y is not a valid", error.substr(0, 16));

  // Find addressable, but it's not allowed to be used for simulation
  EXPECT_EQ(false, objects.VerfiyAddressableForUse("process[unit_tester].a", addressable::kSimulate, error));
  ASSERT_EQ(true, (error.length() > 16));
  EXPECT_EQ("a on process.uni", error.substr(0, 16));

  // Working query
  // Find addressable, but it's not allowed to be used for simulation
  EXPECT_EQ(true, objects.VerfiyAddressableForUse("process[unit_tester].a", addressable::kEstimate, error));
  ASSERT_EQ(true, (error.length() == 0));
}

/**
 * virtual addressable::Type           GetAddressableType(const string& parameter_absolute_name);
 * enum Type {
 * kInvalid      = 0,
 * kSingle       = 1,
 * kMultiple     = 2,
 * kVector       = 3,
 * kStringMap    = 4,
 * kUnsignedMap  = 5,
 * kVectorStringMap = 6
 * };
 */
TEST(Objects, GetAddressableType) {
  Model model;
  model.set_partition_type(PartitionType::kAge);
  Objects& objects = model.objects();
  ASSERT_NE(nullptr, &objects);

  string error = "";
  string parameter = "";

  // Now, we need to create an object and try to reference addressables
  processes::UnitTester* unit_tester = new processes::UnitTester(&model);
  model.managers().process()->AddObject(unit_tester);
  unit_tester->parameters().Add(PARAM_LABEL, "unit_tester", __FILE__, __LINE__);
  unit_tester->parameters().Add(PARAM_TYPE, PARAM_NOP, __FILE__, __LINE__);
  model.managers().process()->Validate(&model);

  EXPECT_EQ(addressable::kSingle,       objects.GetAddressableType("process[unit_tester].a"));
  EXPECT_EQ(addressable::kVector,       objects.GetAddressableType("process[unit_tester].b"));
  EXPECT_EQ(addressable::kSingle,       objects.GetAddressableType("process[unit_tester].b{1}"));
  EXPECT_EQ(addressable::kMultiple,     objects.GetAddressableType("process[unit_tester].b{1,2}"));
  EXPECT_EQ(addressable::kUnsignedMap,  objects.GetAddressableType("process[unit_tester].c"));
  EXPECT_EQ(addressable::kSingle,       objects.GetAddressableType("process[unit_tester].c{1990}"));
  EXPECT_EQ(addressable::kMultiple,     objects.GetAddressableType("process[unit_tester].c{1990,1991}"));
  EXPECT_EQ(addressable::kStringMap,    objects.GetAddressableType("process[unit_tester].d"));
  EXPECT_EQ(addressable::kSingle,       objects.GetAddressableType("process[unit_tester].d{a}"));
  EXPECT_EQ(addressable::kMultiple,     objects.GetAddressableType("process[unit_tester].d{a, b}"));
}

/**
 * virtual Double* GetAddressable(const string& addressable_absolute_name);
 */
TEST(Objects, GetAddressable) {
  Model model;
  model.set_partition_type(PartitionType::kAge);
  Objects& objects = model.objects();
  ASSERT_NE(nullptr, &objects);

  string error = "";
  string parameter = "";

  // Now, we need to create an object and try to reference addressables
  processes::UnitTester* unit_tester = new processes::UnitTester(&model);
  model.managers().process()->AddObject(unit_tester);
  unit_tester->parameters().Add(PARAM_LABEL, "unit_tester", __FILE__, __LINE__);
  unit_tester->parameters().Add(PARAM_TYPE, PARAM_NOP, __FILE__, __LINE__);
  model.managers().process()->Validate(&model);

  EXPECT_EQ(addressable::kSingle,       objects.GetAddressableType("process[unit_tester].a"));
  ASSERT_NE(nullptr, objects.GetAddressable("process[unit_tester].a"));
  ASSERT_EQ(&unit_tester->addressable_, objects.GetAddressable("process[unit_tester].a"));
  EXPECT_DOUBLE_EQ(1.0, *objects.GetAddressable("process[unit_tester].a"));

  EXPECT_EQ(addressable::kSingle,       objects.GetAddressableType("process[unit_tester].b{2}"));
  ASSERT_NE(nullptr, objects.GetAddressable("process[unit_tester].b{2}"));
  ASSERT_EQ(&unit_tester->addressable_v_[1], objects.GetAddressable("process[unit_tester].b{2}"));
  EXPECT_DOUBLE_EQ(1.5, *objects.GetAddressable("process[unit_tester].b{2}"));

  EXPECT_EQ(addressable::kSingle,       objects.GetAddressableType("process[unit_tester].c{1992}"));
  ASSERT_NE(nullptr, objects.GetAddressable("process[unit_tester].c{1992}"));
  ASSERT_EQ(&unit_tester->addressable_um_[1992], objects.GetAddressable("process[unit_tester].c{1992}"));
  EXPECT_DOUBLE_EQ(2.0, *objects.GetAddressable("process[unit_tester].c{1992}"));

  EXPECT_EQ(addressable::kSingle,       objects.GetAddressableType("process[unit_tester].d{b}"));
  ASSERT_NE(nullptr, objects.GetAddressable("process[unit_tester].d{b}"));
  ASSERT_EQ(&unit_tester->addressable_sm_["b"], objects.GetAddressable("process[unit_tester].d{b}"));
  EXPECT_DOUBLE_EQ(1.5, *objects.GetAddressable("process[unit_tester].d{b}"));
}

/**
 * virtual Double* GetAddressable(const string& addressable_absolute_name);
 */
TEST(Objects, GetAddressable_Fail) {
  Model model;
  model.set_partition_type(PartitionType::kAge);
  Objects& objects = model.objects();
  ASSERT_NE(nullptr, &objects);

  string error = "";
  string parameter = "";

  // Now, we need to create an object and try to reference addressables
  processes::UnitTester* unit_tester = new processes::UnitTester(&model);
  model.managers().process()->AddObject(unit_tester);
  unit_tester->parameters().Add(PARAM_LABEL, "unit_tester", __FILE__, __LINE__);
  unit_tester->parameters().Add(PARAM_TYPE, PARAM_NOP, __FILE__, __LINE__);
  model.managers().process()->Validate(&model);

  EXPECT_THROW(objects.GetAddressable("bad[label].addressable"), std::string);
  EXPECT_THROW(objects.GetAddressable("process[bad_label].addressable"), std::string);
  EXPECT_THROW(objects.GetAddressable("process[unit_tester].z"), std::string);
}

/**
 * virtual vector<Double*>* GetAddressables(const string& addressable_absolute_name);
 */
TEST(Objects, GetAddressables) {
  Model model;
  model.set_partition_type(PartitionType::kAge);
  Objects& objects = model.objects();
  ASSERT_NE(nullptr, &objects);

  string error = "";
  string parameter = "";

  // Now, we need to create an object and try to reference addressables
  processes::UnitTester* unit_tester = new processes::UnitTester(&model);
  model.managers().process()->AddObject(unit_tester);
  unit_tester->parameters().Add(PARAM_LABEL, "unit_tester", __FILE__, __LINE__);
  unit_tester->parameters().Add(PARAM_TYPE, PARAM_NOP, __FILE__, __LINE__);
  model.managers().process()->Validate(&model);

  EXPECT_EQ(addressable::kMultiple,     objects.GetAddressableType("process[unit_tester].b{1,2}"));
  ASSERT_NE(nullptr, objects.GetAddressables("process[unit_tester].b{1,2}"));
  EXPECT_DOUBLE_EQ(1.0, *(*objects.GetAddressables("process[unit_tester].b{1,2}"))[0]);
  EXPECT_DOUBLE_EQ(1.5, *(*objects.GetAddressables("process[unit_tester].b{1,2}"))[1]);

  EXPECT_EQ(addressable::kMultiple,     objects.GetAddressableType("process[unit_tester].c{1990,1991}"));
  ASSERT_NE(nullptr, objects.GetAddressables("process[unit_tester].c{1990,1991}"));
  EXPECT_DOUBLE_EQ(1.0, *(*objects.GetAddressables("process[unit_tester].c{1990,1991}"))[0]);
  EXPECT_DOUBLE_EQ(1.5, *(*objects.GetAddressables("process[unit_tester].c{1990,1991}"))[1]);

  EXPECT_EQ(addressable::kMultiple,     objects.GetAddressableType("process[unit_tester].d{a, b}"));
  ASSERT_NE(nullptr, objects.GetAddressables("process[unit_tester].d{a, b}"));
  EXPECT_DOUBLE_EQ(1.0, *(*objects.GetAddressables("process[unit_tester].d{a, b}"))[0]);
  EXPECT_DOUBLE_EQ(1.5, *(*objects.GetAddressables("process[unit_tester].d{a, b}"))[1]);
}

/**
 * virtual map<unsigned, Double>* GetAddressableUMap(const string& addressable_absolute_name);
 */
TEST(Objects, GetAddressableUMap) {
  Model model;
  model.set_partition_type(PartitionType::kAge);
  Objects& objects = model.objects();
  ASSERT_NE(nullptr, &objects);

  string error = "";
  string parameter = "";

  // Now, we need to create an object and try to reference addressables
  processes::UnitTester* unit_tester = new processes::UnitTester(&model);
  model.managers().process()->AddObject(unit_tester);
  unit_tester->parameters().Add(PARAM_LABEL, "unit_tester", __FILE__, __LINE__);
  unit_tester->parameters().Add(PARAM_TYPE, PARAM_NOP, __FILE__, __LINE__);
  model.managers().process()->Validate(&model);

  EXPECT_EQ(addressable::kUnsignedMap,  objects.GetAddressableType("process[unit_tester].c"));
  ASSERT_NE(nullptr, objects.GetAddressableUMap("process[unit_tester].c"));
  ASSERT_EQ(&unit_tester->addressable_um_, objects.GetAddressableUMap("process[unit_tester].c"));
  ASSERT_EQ(3u, objects.GetAddressableUMap("process[unit_tester].c")->size());
  EXPECT_DOUBLE_EQ(1.0, (*objects.GetAddressableUMap("process[unit_tester].c"))[1990]);
  EXPECT_DOUBLE_EQ(1.5, (*objects.GetAddressableUMap("process[unit_tester].c"))[1991]);
  EXPECT_DOUBLE_EQ(2.0, (*objects.GetAddressableUMap("process[unit_tester].c"))[1992]);
}

/**
 * virtual OrderedMap<string, Double>* GetAddressableSMap(const string& addressable_absolute_name);
 */
TEST(Objects, GetAddressableSMap) {
  Model model;
  model.set_partition_type(PartitionType::kAge);
  Objects& objects = model.objects();
  ASSERT_NE(nullptr, &objects);

  string error = "";
  string parameter = "";

  // Now, we need to create an object and try to reference addressables
  processes::UnitTester* unit_tester = new processes::UnitTester(&model);
  model.managers().process()->AddObject(unit_tester);
  unit_tester->parameters().Add(PARAM_LABEL, "unit_tester", __FILE__, __LINE__);
  unit_tester->parameters().Add(PARAM_TYPE, PARAM_NOP, __FILE__, __LINE__);
  model.managers().process()->Validate(&model);

  EXPECT_EQ(addressable::kStringMap,    objects.GetAddressableType("process[unit_tester].d"));
  ASSERT_NE(nullptr, objects.GetAddressableSMap("process[unit_tester].d"));
  ASSERT_EQ(&unit_tester->addressable_sm_, objects.GetAddressableSMap("process[unit_tester].d"));
  ASSERT_EQ(3u, objects.GetAddressableSMap("process[unit_tester].d")->size());
  EXPECT_DOUBLE_EQ(1.0, (*objects.GetAddressableSMap("process[unit_tester].d"))["a"]);
  EXPECT_DOUBLE_EQ(1.5, (*objects.GetAddressableSMap("process[unit_tester].d"))["b"]);
  EXPECT_DOUBLE_EQ(2.0, (*objects.GetAddressableSMap("process[unit_tester].d"))["c"]);
}

/**
 * virtual vector<Double>* GetAddressableVector(const string& addressable_absolute_name);
 */
TEST(Objects, GetAddressableVector) {
  Model model;
  model.set_partition_type(PartitionType::kAge);
  Objects& objects = model.objects();
  ASSERT_NE(nullptr, &objects);

  string error = "";
  string parameter = "";

  // Now, we need to create an object and try to reference addressables
  processes::UnitTester* unit_tester = new processes::UnitTester(&model);
  model.managers().process()->AddObject(unit_tester);
  unit_tester->parameters().Add(PARAM_LABEL, "unit_tester", __FILE__, __LINE__);
  unit_tester->parameters().Add(PARAM_TYPE, PARAM_NOP, __FILE__, __LINE__);
  model.managers().process()->Validate(&model);

  EXPECT_EQ(addressable::kVector,  objects.GetAddressableType("process[unit_tester].b"));
  ASSERT_NE(nullptr, objects.GetAddressableVector("process[unit_tester].b"));
  ASSERT_EQ(&unit_tester->addressable_v_, objects.GetAddressableVector("process[unit_tester].b"));
  ASSERT_EQ(3u, objects.GetAddressableVector("process[unit_tester].b")->size());
  ASSERT_EQ(&unit_tester->addressable_v_[0], objects.GetAddressable("process[unit_tester].b{1}"));
  ASSERT_EQ(&unit_tester->addressable_v_[1], objects.GetAddressable("process[unit_tester].b{2}"));
  ASSERT_EQ(&unit_tester->addressable_v_[2], objects.GetAddressable("process[unit_tester].b{3}"));
  EXPECT_DOUBLE_EQ(1.0, (*objects.GetAddressableVector("process[unit_tester].b"))[0]);
  EXPECT_DOUBLE_EQ(1.5, (*objects.GetAddressableVector("process[unit_tester].b"))[1]);
  EXPECT_DOUBLE_EQ(2.0, (*objects.GetAddressableVector("process[unit_tester].b"))[2]);

  EXPECT_DOUBLE_EQ(1.0, *objects.GetAddressable("process[unit_tester].b{1}"));
  EXPECT_DOUBLE_EQ(1.5, *objects.GetAddressable("process[unit_tester].b{2}"));
  EXPECT_DOUBLE_EQ(2.0, *objects.GetAddressable("process[unit_tester].b{3}"));
}

/**
 * void ExplodeString(const string& parameter_absolute_name, string &type, string& label, string& addressable, string& index);
 *
 * First we define a simple Mock class to change the original function from Protected to Public
 */
class MockObjectsExplodeString : public niwa::Objects {
public:
  MockObjectsExplodeString(Model* model) : niwa::Objects(model) { };
  virtual ~MockObjectsExplodeString() = default;
  void MockExplodeString(const string& parameter_absolute_name, string &type, string& label, string& addressable, string& index) {
    this->ExplodeString(parameter_absolute_name, type, label, addressable, index);
  }
};

TEST(Objects, ExplodeString) {
  Model model;
  model.set_partition_type(PartitionType::kAge);
  MockObjectsExplodeString objects(&model);

  string type = "", label = "", addressable = "", index = "";
  string abs = "";

  // Check
  abs = "type[label].addressable";
  objects.MockExplodeString(abs, type, label, addressable, index);
  EXPECT_EQ("type", type);
  EXPECT_EQ("label", label);
  EXPECT_EQ("addressable", addressable);
  EXPECT_EQ("", index);

  // Check
  abs = "typei[labeli].addressablei{index}";
  objects.MockExplodeString(abs, type, label, addressable, index);
  EXPECT_EQ("typei", type);
  EXPECT_EQ("labeli", label);
  EXPECT_EQ("addressablei", addressable);
  EXPECT_EQ("index", index);

  // Check
  abs = "process[unit_tester].a";
  objects.MockExplodeString(abs, type, label, addressable, index);
  EXPECT_EQ("process", type);
  EXPECT_EQ("unit_tester", label);
  EXPECT_EQ("a", addressable);
  EXPECT_EQ("", index);

  // Check
  abs = "process[unit_tester].a{1,2}";
  objects.MockExplodeString(abs, type, label, addressable, index);
  EXPECT_EQ("process", type);
  EXPECT_EQ("unit_tester", label);
  EXPECT_EQ("a", addressable);
  EXPECT_EQ("1,2", index);
}

/**
 * void ImplodeString(const string& type, const string& label, const string& parameter, const string& index, string& target_parameter);
 *
 * First we define a simple Mock class to change the original function from Protected to Public
 */
class MockObjectsImplodeString : public niwa::Objects {
public:
  MockObjectsImplodeString(Model* model) : niwa::Objects(model) { };
  virtual ~MockObjectsImplodeString() = default;
  void MockImplodeString(const string& type, const string& label, const string& parameter, const string& index, string& target_parameter) {
    this->ImplodeString(type, label, parameter, index, target_parameter);
  }
};

TEST(Objects, ImplodeString) {
  Model model;
  model.set_partition_type(PartitionType::kAge);
  MockObjectsImplodeString objects(&model);

  string type = "", label = "", addressable = "", index = "";
  string result = "";

  // Check
  type = "type";
  label = "label";
  addressable = "addressable";
  index = "";
  objects.MockImplodeString(type, label, addressable, index, result);
  EXPECT_EQ("type[label].addressable", result);

  // Check
  type = "typei";
  label = "labeli";
  addressable = "addressablei";
  index = "index";
  objects.MockImplodeString(type, label, addressable, index, result);
  EXPECT_EQ("typei[labeli].addressablei{index}", result);

  // Check
  type = "process";
  label = "unit_tester";
  addressable = "a";
  index = "";
  objects.MockImplodeString(type, label, addressable, index, result);
  EXPECT_EQ("process[unit_tester].a", result);

  // Check
  type = "process";
  label = "unit_tester";
  addressable = "a";
  index = "1,2";
  objects.MockImplodeString(type, label, addressable, index, result);
  EXPECT_EQ("process[unit_tester].a{1,2}", result);
}

/**
 * std::pair<string, string>   ExplodeParameterAndIndex(const string& parameter_absolute_name);
 *
 * First we define a simple Mock class to change the original function from Protected to Public
 */
class MockObjectsExplodeParameterAndIndex : public niwa::Objects {
public:
  MockObjectsExplodeParameterAndIndex(Model* model) : niwa::Objects(model) { };
  virtual ~MockObjectsExplodeParameterAndIndex() = default;
  std::pair<string, string> MockExplodeParameterAndIndex(const string& parameter_absolute_name) {
    return this->ExplodeParameterAndIndex(parameter_absolute_name);
  }
};

TEST(Objects, ExplodeParameterAndIndex) {
  Model model;
  model.set_partition_type(PartitionType::kAge);
  MockObjectsExplodeParameterAndIndex objects(&model);

  string abs = "";
  std::pair<string, string> result;

  // Check
  abs = "type[label].addressable";
  result = objects.MockExplodeParameterAndIndex(abs);
  EXPECT_EQ("addressable", result.first);
  EXPECT_EQ("", result.second);

  // Check
  abs = "type[label].addressablei{index}";
  result = objects.MockExplodeParameterAndIndex(abs);
  EXPECT_EQ("addressablei", result.first);
  EXPECT_EQ("index", result.second);

  // Check
  abs = "process[unit_tester].a{1,2}";
  result = objects.MockExplodeParameterAndIndex(abs);
  EXPECT_EQ("a", result.first);
  EXPECT_EQ("1,2", result.second);

  // Check
  abs = "process[unit_tester].a{1, 2}";
  result = objects.MockExplodeParameterAndIndex(abs);
  EXPECT_EQ("a", result.first);
  EXPECT_EQ("1, 2", result.second);

  // Check
  abs = "process[unit_tester].a{1:3}";
  result = objects.MockExplodeParameterAndIndex(abs);
  EXPECT_EQ("a", result.first);
  EXPECT_EQ("1:3", result.second);
}

/**
 * virtual base::Object*               FindObject(const string& parameter_absolute_name);
 */
TEST(Objects, FindObject) {
  Model model;
  model.set_partition_type(PartitionType::kAge);
  Objects& objects = model.objects();
  ASSERT_NE(nullptr, &objects);

  string error = "";
  string parameter = "";

  // Create a bunch of processes with different names to try and find
  vector<string> names = { "unit0", "unit01", "unit02", "unit03", "unit_04", "unit-05", "unit.06" };
  vector<processes::UnitTester*> objs;
  for (auto& name : names) {
    processes::UnitTester* unit_tester = new processes::UnitTester(&model);
    model.managers().process()->AddObject(unit_tester);
    unit_tester->parameters().Add(PARAM_LABEL, name, __FILE__, __LINE__);
    unit_tester->parameters().Add(PARAM_TYPE, PARAM_NOP, __FILE__, __LINE__);

    objs.push_back(unit_tester);
  }
  model.managers().process()->Validate(&model);

  ASSERT_EQ(objs[0], objects.FindObject("process[unit0].addressable"));
  ASSERT_EQ(objs[1], objects.FindObject("process[unit01].addressable"));
  ASSERT_EQ(objs[2], objects.FindObject("process[unit02].addressable"));
  ASSERT_EQ(objs[3], objects.FindObject("process[unit03].addressable"));
  ASSERT_EQ(objs[4], objects.FindObject("process[unit_04].addressable"));
  ASSERT_EQ(objs[5], objects.FindObject("process[unit-05].addressable"));
  ASSERT_EQ(objs[6], objects.FindObject("process[unit.06].addressable"));
}

/**
 * virtual base::Object*               FindObject(const string& parameter_absolute_name);
 */
TEST(Objects, FindObject_Fail) {
  Model model;
  model.set_partition_type(PartitionType::kAge);
  Objects& objects = model.objects();
  ASSERT_NE(nullptr, &objects);

  ASSERT_THROW(objects.FindObject("process"), std::string);
  ASSERT_THROW(objects.FindObject("process[unit01]"), std::string);
  ASSERT_THROW(objects.FindObject("process[unit02].addressable"), std::string);
  ASSERT_THROW(objects.FindObject("process[unit03].addressable{index}"), std::string);
}

} /* namespace niwa */

#endif /* TESTMODE */
