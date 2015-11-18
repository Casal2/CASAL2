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
#include "Object.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

// namespaces
namespace niwa {
namespace base {

/**
 * This test is checking that we can get a basic estimable
 * and the error handling is right
 */
class TestObject1 : public Object {
public:
  void Reset() override final { };

  TestObject1() {
    RegisterAsEstimable("lemon", &estimable);
  }

  double  estimable = 0.0;
};

TEST(Object, Standard_Double_Estimable) {
  TestObject1 object;

  EXPECT_EQ(Estimable::kSingle, object.GetEstimableType("lemon"));
  EXPECT_EQ(&object.estimable, object.GetEstimable("lemon"));

  EXPECT_THROW(object.GetEstimableType("apple"), std::string);
  EXPECT_THROW(object.GetEstimable("apple"), std::string);
}

/**
 * Test 2
 */
class TestObject2 : public Object {
public:
  void Reset() override final { };

  TestObject2() {
    RegisterAsEstimable("lemon", &estimables_);
    estimables_.push_back(2.5);
    estimables_.push_back(5.0);
    estimables_.push_back(7.5);
    estimables_.push_back(9.0);
  }

  vector<double>  estimables_;
};

TEST(Object, Vector_Double_Estimable) {
  TestObject2 object;

  EXPECT_EQ(Estimable::kVector, object.GetEstimableType("lemon"));
  EXPECT_EQ(&object.estimables_, object.GetEstimableVector("lemon"));
  EXPECT_EQ(&object.estimables_[0], object.GetEstimable("lemon", "1"));
  EXPECT_EQ(&object.estimables_[1], object.GetEstimable("lemon", "2"));
  EXPECT_EQ(&object.estimables_[2], object.GetEstimable("lemon", "3"));

  EXPECT_THROW(object.GetEstimableType("apple"), std::string);
  EXPECT_THROW(object.GetEstimable("apple"), std::string);
}

/**
 * Test 3
 */
class TestObject3 : public Object {
public:
  void Reset() override final { };

  TestObject3() {
    RegisterAsEstimable("lemon", &estimables_);

    estimables_["male"] = 1.0;
    estimables_["female"] = 1.0;
  }

  OrderedMap<string, double>  estimables_;
};

TEST(Object, StringMap_Double_Estimable) {
  TestObject3 object;

  EXPECT_EQ(Estimable::kStringMap, object.GetEstimableType("lemon"));
  EXPECT_EQ(&object.estimables_, object.GetEstimableSMap("lemon"));
  EXPECT_EQ(&object.estimables_["male"], object.GetEstimable("lemon", "male"));
  EXPECT_EQ(&object.estimables_["female"], object.GetEstimable("lemon", "female"));
  EXPECT_EQ(1.0, *object.GetEstimable("lemon", "male"));
  EXPECT_EQ(1.0, *object.GetEstimable("lemon", "female"));

  EXPECT_THROW(object.GetEstimableType("apple"), std::string);
  EXPECT_THROW(object.GetEstimable("apple"), std::string);
}

/**
 * Test 4
 */
class TestObject4 : public Object {
public:
  void Reset() override final { };

  TestObject4() {
    RegisterAsEstimable("lemon", &estimables_);

    estimables_[100] = 1.0;
    estimables_[101] = 1.0;
  }

  map<unsigned, double>  estimables_;
};

TEST(Object, UnsignedMap_Double_Estimable) {
  TestObject4 object;

  EXPECT_EQ(Estimable::kUnsignedMap, object.GetEstimableType("lemon"));
  EXPECT_EQ(&object.estimables_, object.GetEstimableUMap("lemon"));
  EXPECT_EQ(&object.estimables_[100], object.GetEstimable("lemon", "100"));
  EXPECT_EQ(&object.estimables_[101], object.GetEstimable("lemon", "101"));
  EXPECT_EQ(1.0, *object.GetEstimable("lemon", "100"));
  EXPECT_EQ(1.0, *object.GetEstimable("lemon", "101"));

  EXPECT_THROW(object.GetEstimableType("apple"), std::string);
  EXPECT_THROW(object.GetEstimable("apple"), std::string);
}

/**
 * Test 5
 */
class TestObject5 : public Object {
public:
  void Reset() override final { };

  TestObject5() {
    RegisterAsEstimable(&estimables_);

    estimables_["lemon"].push_back(1.0);
  }

  map<string, vector<double> >  estimables_;
};

TEST(Object, UnnamedVectorMap_Double_Estimable) {
  TestObject5 object;

  EXPECT_EQ(Estimable::kVectorStringMap, object.GetEstimableType("lemon"));
  ASSERT_TRUE(object.estimables_.find("lemon") != object.estimables_.end());
  EXPECT_EQ(&object.estimables_["lemon"], object.GetEstimableVector("lemon"));
  ASSERT_TRUE(object.estimables_["lemon"].size() == 1);
  EXPECT_EQ(1.0, object.estimables_["lemon"][0]);

  EXPECT_THROW(object.GetEstimableType("apple"), std::string);
  EXPECT_THROW(object.GetEstimable("apple"), std::string);
}
/**
 * Test 6
 */
class TestObject6 : public Object {
public:
  void Reset() override final { };

  TestObject6() {
    RegisterAsEstimable(&estimables_);

    estimables_["lemon+apple"].push_back(1.0);
  }

  map<string, vector<double> >  estimables_;
};

TEST(Object, UnnamedVectorMap_Double_Estimable_with_plus) {
  TestObject6 object;

  EXPECT_EQ(Estimable::kVectorStringMap, object.GetEstimableType("lemon+apple"));
  ASSERT_TRUE(object.estimables_.find("lemon+apple") != object.estimables_.end());
  EXPECT_EQ(&object.estimables_["lemon+apple"], object.GetEstimableVector("lemon+apple"));
  ASSERT_TRUE(object.estimables_["lemon+apple"].size() == 1);
  EXPECT_EQ(1.0, object.estimables_["lemon+apple"][0]);

  EXPECT_THROW(object.GetEstimableType("apple"), std::string);
  EXPECT_THROW(object.GetEstimable("apple"), std::string);
}

} /* namespace base */
} /* namespace niwa */
#endif
