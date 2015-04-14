/**
 * @file Logging.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 14/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Logging.h"

#include "GlobalConfiguration/GlobalConfiguration.h"

// namespaces
namespace niwa {

using std::vector;
using std::string;

/**
 * Default constructor
 */
Logging::Logging() {
  // Set our logging level
  GlobalConfigurationPtr config = GlobalConfiguration::Instance();
  string log_level = config->log_level();
  if (log_level == PARAM_TRACE)
    current_log_level_ = logger::Severity::kTrace;
  else if (log_level == PARAM_FINEST)
    current_log_level_ = logger::Severity::kFinest;
  else if (log_level == PARAM_FINE)
    current_log_level_ = logger::Severity::kFine;
  else if (log_level != PARAM_NONE) {
    cout << "The log level provided is an invalid log level. " << log_level << " is not supported" << endl;
    exit(-1);
  }
}

/**
 * Singleton Instance method
 *
 * @return static instance of the logging class
 */
Logging& Logging::Instance() {
  static Logging logging;
  return logging;
}

/**
 *
 */
void Logging::Flush(niwa::logger::Record& record) {
  record.BuildMessage();

  if (record.severity() == logger::Severity::kWarning)
    warnings_.push_back(record.stream().str());
  else if (record.severity() == logger::Severity::kError)
    errors_.push_back(record.stream().str());
  else if (record.severity() == logger::Severity::kFatal || record.severity() == logger::Severity::kCodeError) {
    cout << record.message();
    if (errors_.size() > 0)
      cout << "NOTE: " << errors_.size() << " other errors have been logged above\n";

    cout.flush();
    exit(-1);
  }

  if (static_cast<int>(record.severity()) >= static_cast<int>(current_log_level_)) {
    cout << record.message();
    cout.flush();
  }
}

/**
 *
 */
void Logging::FlushErrors() {
  if (errors_.size() == 0)
    return;

  unsigned to_print = errors_.size() > 10 ? 10 : errors_.size();

  cout << "\n";
  cout << "********************************************************************************\n";
  cout << "********                     SUMMARY OF ERRORS                          ********\n";
  cout << "********************************************************************************\n";
  cout << "Printing " << to_print << " of " << errors_.size() << " errors\n";
  cout << "\n";

  for (unsigned i = 0; i < to_print; ++i) {
    cout << "#" << i << ": " << errors_[i] << "\n";
  }

  cout << "\n";
  cout.flush();
}

} /* namespace niwa */
