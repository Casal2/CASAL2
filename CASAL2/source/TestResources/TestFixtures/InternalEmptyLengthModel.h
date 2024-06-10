/**
 * @file InternalEmptyLengthModel.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 20/08/2013
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This test fixtures is an empty model but it contains all of the
 * methods needed to load strings internally so we don't have to
 * rely on external configuration files when building unit test models.
 */
#ifdef TESTMODE
#ifndef TESTFIXTURES_INTERNALEMPTY_LENGTH_MODEL_H_
#define TESTFIXTURES_INTERNALEMPTY_LENGTH_MODEL_H_

// Headers
#include <string>
#include <vector>

#include "../../ConfigurationLoader/Loader.h"
#include "../../TestResources/TestFixtures/BaseLength.h"

// Namespaces
namespace niwa {
namespace model {
class Length;
}

namespace testfixtures {
using std::string;
using std::vector;
namespace config = niwa::configuration;

/**
 * class definition
 */
class InternalEmptyLengthModel : public testfixtures::BaseLength {
public:
  // methods
  InternalEmptyLengthModel()          = default;
  virtual ~InternalEmptyLengthModel() = default;
  void SetUp() override final;
  void AddConfigurationLine(const string& line, const string& file_name, unsigned line_number);
  void LoadConfiguration();

protected:
  // members
  vector<config::FileLine> configuration_file_;
};

} /* namespace testfixtures */
} /* namespace niwa */
#endif /* INTERNALEMPTYMODEL_H_ */
#endif /* TESTMODE */
