/**
 * @file Base.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 2/09/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
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
#include <string>

// namespaces
namespace niwa {
class Model;

namespace testfixtures {

/**
 * Class definition
 */
class Base : public ::testing::Test {
public:
  Base();
  virtual                     ~Base();
  virtual void                SetUp() override;
  virtual void                TearDown() override;

protected:
  // members
  Model*                      model_;
};

} /* namespace testfixtures */
} /* namespace niwa */
#endif /* TESTMODE */
#endif /* BASE_H_ */
