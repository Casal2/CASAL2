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
#ifndef TESTFIXTURES_BASETHREADED_H_
#define TESTFIXTURES_BASETHREADED_H_
#ifdef TESTMODE

// headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "../../ConfigurationLoader/Loader.h"
#include "../../Model/Model.h"
#include "../../Runner.h"
#include "../../TestResources/TestFixtures/Base.h"

// namespaces
namespace niwa {
namespace testfixtures {

using std::shared_ptr;
using std::string;
using std::vector;
namespace config = niwa::configuration;

/**
 * Class definition
 */
class BaseThreaded : public ::testing::Test {
public:
  BaseThreaded()          = default;
  virtual ~BaseThreaded() = default;
  virtual void SetUp() override;
  virtual void TearDown() override;
  void         AddConfigurationLine(const string& line, const string& file_name, unsigned line_number);
  void         LoadConfiguration();

protected:
  // members
  shared_ptr<Runner>       runner_ = nullptr;
  vector<config::FileLine> configuration_file_;
};

} /* namespace testfixtures */
} /* namespace niwa */
#endif /* TESTMODE */
#endif /* TESTFIXTURES_BASETHREADED_H_ */
