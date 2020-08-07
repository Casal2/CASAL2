/*
 * Constant.Mock.h
 *
 *  Created on: 28/01/2019
 *      Author: Zaita
 */

#ifndef SOURCE_TIMEVARYING_COMMON_CONSTANT_MOCK_H_
#define SOURCE_TIMEVARYING_COMMON_CONSTANT_MOCK_H_

#include "Constant.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace niwa {
namespace timevarying {

class MockConstant : public timevarying::Constant {
public:
  // methods
  MockConstant(Model* model) : Constant(model) { };
  virtual ~MockConstant() = default;

  // setters
  void set_years(vector<unsigned> years) { years_ = years; }
  void set_values(vector<double> values) { values_ = values; }
};

} /* namespace timevarying */
} /* namespace niwa */

#endif /* SOURCE_TIMEVARYING_COMMON_CONSTANT_MOCK_H_ */
