/**
 * @file Base.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2/09/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifndef TESTFIXTURES_BASE_H_
#define TESTFIXTURES_BASE_H_
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
class Base : public ::testing::Test {
public:
  Base()          = default;
  virtual ~Base() = default;
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
