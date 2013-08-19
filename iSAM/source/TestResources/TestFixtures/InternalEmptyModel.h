/**
 * @file InternalEmptyModel.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 20/08/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This test fixtures is an empty model but it contains all of the
 * methods needed to load strings internally so we don't have to
 * rely on external configuration files when building unit test models.
 */
#ifdef TESTMODE
#ifndef TESTFIXTURES_INTERNALEMPTYMODEL_H_
#define TESTFIXTURES_INTERNALEMPTYMODEL_H_

// Headers
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "ConfigurationLoader/Loader.h"

// Namespaces
namespace isam {
namespace testfixtures {

using std::string;
using std::vector;
namespace config = isam::configuration;

/**
 * class definition
 */
class InternalEmptyModel : public ::testing::Test  {
public:
  // methods
  InternalEmptyModel() { };
  virtual                     ~InternalEmptyModel() { };
  virtual void                SetUp();
  virtual void                TearDown();
  void                        AddConfigurationLine(const string& line, const string& file_name, unsigned line_number);
  void                        LoadConfiguration();

protected:
  // members
  vector<config::FileLine>    configuration_file_;
};

} /* namespace testfixtures */
} /* namespace isam */
#endif /* INTERNALEMPTYMODEL_H_ */
#endif /* TESTMODE */
