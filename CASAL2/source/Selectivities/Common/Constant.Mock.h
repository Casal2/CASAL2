/**
 * @file Objects.Mock.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 28/01/2019
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 */

#ifndef SOURCE_SELECTIVITIES_COMMON_CONSTANT_MOCK_H_
#define SOURCE_SELECTIVITIES_COMMON_CONSTANT_MOCK_H_

// headers
#include <gmock/gmock.h>

#include "Constant.h"

// namespaces
namespace niwa {
namespace selectivities {

// class definition
class MockConstant : public selectivities::Constant {
public:
  MockConstant(shared_ptr<Model> model) : selectivities::Constant(model) {}
  virtual ~MockConstant() = default;
  MOCK_METHOD2(GetAgeResult, double(unsigned age, AgeLength* age_length));
};

} /* namespace selectivities */
} /* namespace niwa */

#endif /* SOURCE_SELECTIVITIES_COMMON_CONSTANT_MOCK_H_ */
