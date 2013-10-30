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

#include "Utilities/RunMode.h"

// Namespaces
namespace isam {
namespace utilities {

using std::string;
using std::map;

/**
 * Class definition
 */
class CommandLineParser {
public:
  // Methods
  CommandLineParser() = default;
  virtual                     ~CommandLineParser() = default;
  void                        Parse(int argc, const char* argv[]);

  // Accessors
  string                      command_line_usage() { return command_line_usage_; }
  RunMode::Type               run_mode() const { return run_mode_; }
  map<string, string>         override_values() const { return override_values_; }

private:
  // Members
  RunMode::Type               run_mode_ = RunMode::kInvalid;
  string                      command_line_usage_   = "";
  map<string, string>         override_values_;
};

} /* namespace utilities */
} /* namespace isam */
#endif /* COMMANDLINEPARSER_H_ */
