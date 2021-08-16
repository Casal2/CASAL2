/**
 * @file Objects.Mock.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/01/2019
 * @section LICENSE
 *
 * Copyright NIWA Science 2019 - www.niwa.co.nz
 *
 */
#ifndef SOURCE_MODEL_OBJECTS_MOCK_H_
#define SOURCE_MODEL_OBJECTS_MOCK_H_
#ifdef TESTMODE

// headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <map>

#include "Objects.h"

// namespaces
namespace niwa {
using std::map;

/**
 * Create our MockObjects Class
 */
class MockObjects : public niwa::Objects {
public:
  MockObjects(shared_ptr<Model> model) : niwa::Objects(model){};
  virtual ~MockObjects() = default;

  MOCK_METHOD3(VerifyAddressableForUse, bool(const string&, addressable::Usage, string&));
  MOCK_METHOD1(GetAddressableType, addressable::Type(const string&));
  MOCK_METHOD1(GetAddressable, double*(const string&));
  MOCK_METHOD1(GetAddressables, vector<Double*>*(const string&));
  MOCK_METHOD1(GetAddressableUMap, map<unsigned, double>*(const string&));
  MOCK_METHOD1(GetAddressableSMap, OrderedMap<string, double>*(const string&));
  MOCK_METHOD1(GetAddressableVector, vector<Double>*(const string&));
  MOCK_METHOD1(FindObject, base::Object*(const string&));

  MOCK_METHOD1(FindObjectOrNull, base::Object*(const string&));
  MOCK_METHOD5(ExplodeString, void(const string&, string&, string&, string&, string&));
  MOCK_METHOD5(ImplodeString, void(const string&, const string&, const string&, const string&, string&));
  MOCK_METHOD1(ExplodeParameterAndIndex, std::pair<string, string>(const string&));
};

} /* namespace niwa */

#endif /* TESTMODE */
#endif /* SOURCE_MODEL_OBJECTS_MOCK_H_ */
