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

#ifndef SOURCE_SELECTIVITIES_COMMON_CONSTANT_MOCK_H_
#define SOURCE_SELECTIVITIES_COMMON_CONSTANT_MOCK_H_


// headers
#include "Constant.h"

#include <gmock/gmock.h>

// namespaces
namespace niwa {
namespace selectivities {

// class definition
class MockConstant : public selectivities::Constant {
public:
  MockConstant(Model* model) : selectivities::Constant(model) { }
  virtual ~MockConstant() = default;
  MOCK_CONST_METHOD2(GetAgeResult, double(unsigned age, AgeLength* age_length));
};

} /* namespace selectivities */
} /* namespace niwa */

#endif /* SOURCE_SELECTIVITIES_COMMON_CONSTANT_MOCK_H_ */
