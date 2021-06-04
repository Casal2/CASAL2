/**
 * @file Object.Test.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 10/11/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifdef TESTMODE

// headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Object.h"

// namespaces
namespace niwa {
namespace base {

/**
 * This test is checking that we can get a basic addressable
 * and the error handling is right
 */
class TestObject1 : public Object {
public:
  void Reset() override final{};

  TestObject1() { RegisterAsAddressable("lemon", &addressable); }

  double addressable = 0.0;
};

TEST(Object, Standard_Double_Addressable) {
  TestObject1 object;

  EXPECT_EQ(addressable::kSingle, object.GetAddressableType("lemon"));
  EXPECT_EQ(&object.addressable, object.GetAddressable("lemon"));

  EXPECT_THROW(object.GetAddressableType("apple"), std::string);
  EXPECT_THROW(object.GetAddressable("apple"), std::string);
}

/**
 * Test 2
 */
class TestObject2 : public Object {
public:
  void Reset() override final{};

  TestObject2() {
    RegisterAsAddressable("lemon", &addressables_);
    addressables_.push_back(2.5);
    addressables_.push_back(5.0);
    addressables_.push_back(7.5);
    addressables_.push_back(9.0);
  }

  vector<double> addressables_;
};

TEST(Object, Vector_Double_Addressable) {
  TestObject2 object;

  EXPECT_EQ(addressable::kVector, object.GetAddressableType("lemon"));
  EXPECT_EQ(&object.addressables_, object.GetAddressableVector("lemon"));
  EXPECT_EQ(&object.addressables_[0], object.GetAddressable("lemon", "1"));
  EXPECT_EQ(&object.addressables_[1], object.GetAddressable("lemon", "2"));
  EXPECT_EQ(&object.addressables_[2], object.GetAddressable("lemon", "3"));

  EXPECT_THROW(object.GetAddressableType("apple"), std::string);
  EXPECT_THROW(object.GetAddressable("apple"), std::string);
}

/**
 * Test 3
 */
class TestObject3 : public Object {
public:
  void Reset() override final{};

  TestObject3() {
    RegisterAsAddressable("lemon", &addressables_);

    addressables_["male"]   = 1.0;
    addressables_["female"] = 1.0;
  }

  OrderedMap<string, double> addressables_;
};

TEST(Object, StringMap_Double_Addressable) {
  TestObject3 object;

  EXPECT_EQ(addressable::kStringMap, object.GetAddressableType("lemon"));
  EXPECT_EQ(&object.addressables_, object.GetAddressableSMap("lemon"));
  EXPECT_EQ(&object.addressables_["male"], object.GetAddressable("lemon", "male"));
  EXPECT_EQ(&object.addressables_["female"], object.GetAddressable("lemon", "female"));
  EXPECT_EQ(1.0, *object.GetAddressable("lemon", "male"));
  EXPECT_EQ(1.0, *object.GetAddressable("lemon", "female"));

  EXPECT_THROW(object.GetAddressableType("apple"), std::string);
  EXPECT_THROW(object.GetAddressable("apple"), std::string);
}

/**
 * Test 4
 */
class TestObject4 : public Object {
public:
  void Reset() override final{};

  TestObject4() {
    RegisterAsAddressable("lemon", &addressables_);

    addressables_[100] = 1.0;
    addressables_[101] = 1.0;
  }

  map<unsigned, double> addressables_;
};

TEST(Object, UnsignedMap_Double_Addressable) {
  TestObject4 object;

  EXPECT_EQ(addressable::kUnsignedMap, object.GetAddressableType("lemon"));
  EXPECT_EQ(&object.addressables_, object.GetAddressableUMap("lemon"));
  EXPECT_EQ(&object.addressables_[100], object.GetAddressable("lemon", "100"));
  EXPECT_EQ(&object.addressables_[101], object.GetAddressable("lemon", "101"));
  EXPECT_EQ(1.0, *object.GetAddressable("lemon", "100"));
  EXPECT_EQ(1.0, *object.GetAddressable("lemon", "101"));

  EXPECT_THROW(object.GetAddressableType("apple"), std::string);
  EXPECT_THROW(object.GetAddressable("apple"), std::string);
}

/**
 * Test 5
 */
class TestObject5 : public Object {
public:
  void Reset() override final{};

  TestObject5() {
    RegisterAsAddressable(&addressables_);

    addressables_["lemon"].push_back(1.0);
  }

  map<string, vector<double> > addressables_;
};

TEST(Object, UnnamedVectorMap_Double_Addressable) {
  TestObject5 object;

  EXPECT_EQ(addressable::kVectorStringMap, object.GetAddressableType("lemon"));
  ASSERT_TRUE(object.addressables_.find("lemon") != object.addressables_.end());
  EXPECT_EQ(&object.addressables_["lemon"], object.GetAddressableVector("lemon"));
  ASSERT_TRUE(object.addressables_["lemon"].size() == 1);
  EXPECT_EQ(1.0, object.addressables_["lemon"][0]);

  EXPECT_THROW(object.GetAddressableType("apple"), std::string);
  EXPECT_THROW(object.GetAddressable("apple"), std::string);
}
/**
 * Test 6
 */
class TestObject6 : public Object {
public:
  void Reset() override final{};

  TestObject6() {
    RegisterAsAddressable(&addressables_);

    addressables_["lemon+apple"].push_back(1.0);
  }

  map<string, vector<double> > addressables_;
};

TEST(Object, UnnamedVectorMap_Double_Addressable_with_plus) {
  TestObject6 object;

  EXPECT_EQ(addressable::kVectorStringMap, object.GetAddressableType("lemon+apple"));
  ASSERT_TRUE(object.addressables_.find("lemon+apple") != object.addressables_.end());
  EXPECT_EQ(&object.addressables_["lemon+apple"], object.GetAddressableVector("lemon+apple"));
  ASSERT_TRUE(object.addressables_["lemon+apple"].size() == 1);
  EXPECT_EQ(1.0, object.addressables_["lemon+apple"][0]);

  EXPECT_THROW(object.GetAddressableType("apple"), std::string);
  EXPECT_THROW(object.GetAddressable("apple"), std::string);
}

} /* namespace base */
} /* namespace niwa */
#endif
