/**
 * @file BaseLength.h
 * @author  C.Marsh
 * @date 2022
 * @section LICENSE
 *
 * Copyright NIWA Science 2022 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef TESTFIXTURES_BASE_LENGTH_H_
#define TESTFIXTURES_BASE_LENGTH_H_
#ifdef TESTMODE

// headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "../../GlobalConfiguration/GlobalConfiguration.h"
#include "../../Model/Model.h"
#include "../../Runner.h"

// namespaces
namespace niwa {
namespace testfixtures {

using std::shared_ptr;

/**
 * Class definition
 */
class BaseLength : public ::testing::Test {
public:
  BaseLength()          = default;
  virtual ~BaseLength() = default;
  virtual void SetUp() override;
  virtual void TearDown() override;

protected:
  // members
  shared_ptr<Model>   model_         = nullptr;
  GlobalConfiguration global_config_ = GlobalConfiguration();
};

} /* namespace testfixtures */
} /* namespace niwa */
#endif /* TESTMODE */
#endif /* BASE_H_ */
