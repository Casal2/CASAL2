/**
 * @file CommandLineParser.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is responsible for parsing the command line parameters
 * and storing them in the global configuration.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef COMMANDLINEPARSER_H_
#define COMMANDLINEPARSER_H_

// Headers
#include <string>
#include <map>

#include "Model/Model.h"
#include "Utilities/RunParameters.h"

// Namespaces
namespace niwa {
namespace utilities {

using std::string;

/**
 * Class definition
 */
class CommandLineParser {
public:
  // Methods
  CommandLineParser() = default;
  virtual                     ~CommandLineParser() = default;
  void                        Parse(int argc, char* argv[], RunParameters& options);
  // Accessors
  string                      command_line_usage() { return command_line_usage_; }

private:
  // Members
  string                      command_line_usage_   = "";
};

} /* namespace utilities */
} /* namespace niwa */
#endif /* COMMANDLINEPARSER_H_ */
