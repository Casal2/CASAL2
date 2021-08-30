/**
 * @file Logging.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 14/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "Logging.h"

#include "../Model/Model.h"
#include "../Utilities/Exception.h"

// namespaces
namespace niwa {

using std::scoped_lock;
using std::string;
using std::vector;

std::mutex Logging::lock_;

#ifdef TESTMODE
logger::Severity Logging::current_log_level_ = logger::Severity::kWarning;
#else
logger::Severity Logging::current_log_level_ = logger::Severity::kInfo;
#endif

/**
 * Default constructor
 */
Logging::Logging() {}

/**
 * Singleton Instance method
 *
 * @return static instance of the logging class
 */
Logging& Logging::Instance() {
  std::scoped_lock l(lock_);
  static Logging   logging;
  return logging;
}

/**
 * This method sets the log level
 */
void Logging::SetLogLevel(const std::string& log_level) {
  std::scoped_lock l(lock_);

  if (log_level == PARAM_TRACE)
    Logging::current_log_level_ = logger::Severity::kTrace;
  else if (log_level == PARAM_FINEST)
    Logging::current_log_level_ = logger::Severity::kFinest;
  else if (log_level == PARAM_FINE)
    Logging::current_log_level_ = logger::Severity::kFine;
  else if (log_level == PARAM_MEDIUM)
    Logging::current_log_level_ = logger::Severity::kMedium;
  else if (log_level == PARAM_INFO)
    Logging::current_log_level_ = logger::Severity::kInfo;
  else if (log_level == PARAM_IMPORTANT)
    Logging::current_log_level_ = logger::Severity::kImportant;
  else if (log_level == PARAM_WARNING)
    Logging::current_log_level_ = logger::Severity::kWarning;
  else if (log_level != PARAM_NONE) {
    cout << "The log level provided is an invalid log level. " << log_level << " is not supported" << endl;
    exit(-1);
  }
}

#ifdef TESTMODE
/**
 * This method flushes the standard output and standard error
 */
void Logging::Flush(niwa::logger::Record& record) {
  record.BuildMessage();

  if (record.severity() == logger::Severity::kWarning || record.severity() == logger::Severity::kError || record.severity() == logger::Severity::kFatal
      || record.severity() == logger::Severity::kCodeError) {
    cout << "Unit Test has encountered a problem that will cause test failure, unless it's expected\n";
    cout << "record.severity(): " << (int)record.severity() << "\n";
    cout << "failure because of: " << record.message();
    cout.flush();
    throw std::string(record.message());
  }

  if (static_cast<int>(record.severity()) >= static_cast<int>(current_log_level_)) {
    cerr << record.message();
    cerr.flush();
  }
}
#else

void Logging::Flush(niwa::logger::Record& record) {
  std::scoped_lock l(lock_);

  record.BuildMessage();

  if (record.severity() == logger::Severity::kInfo)
    info_.push_back(record.stream().str());
  if (record.severity() == logger::Severity::kImportant)
    important_.push_back(record.stream().str());
  else if (record.severity() == logger::Severity::kWarning)
    warnings_.push_back(record.stream().str());
  else if (record.severity() == logger::Severity::kError)
    errors_.push_back(record.stream().str());
  else if (record.severity() == logger::Severity::kFatal || record.severity() == logger::Severity::kCodeError) {
    cerr << record.message();
    if (errors_.size() > 0)
      cerr << "NOTE: " << errors_.size() << " non-fatal errors were also recorded\n";

    cerr.flush();
    exit(-1);
  }

  if (static_cast<int>(record.severity()) >= static_cast<int>(current_log_level_)) {
    cerr << record.message();
    cerr.flush();
  }
}
#endif

/**
 * This method summarises the errors and flushes the standard output
 */
void Logging::FlushErrors() {
  std::scoped_lock l(lock_);
  if (errors_.size() == 0) {
    return;
  }

  unsigned to_print = errors_.size() > 10 ? 10 : errors_.size();

  cout << "\n";
  cout << "ERROR: Printing " << to_print << " of " << errors_.size() << " errors\n";
  cout << "\n";

  for (unsigned i = 0; i < to_print; ++i) {
    cout << "#" << i + 1 << ": " << errors_[i] << "\n";
  }

  cout << "\n";
  cout.flush();

  errors_.clear();
}

/**
 * A info report that is compatable with the Casal2 R package
 */
void Logging::FlushInfo() {
  std::scoped_lock l(lock_);
  if (info_.size() == 0) {
    return;
  }

  cout << "*info[messages_encountered]" << REPORT_EOL;
  cout << "Values " << REPORT_R_STRING_VECTOR << REPORT_EOL;
  for (unsigned i = 0; i < info_.size(); ++i) cout << info_[i] << REPORT_EOL;
  cout << REPORT_END << REPORT_EOL << REPORT_EOL;
  cout.flush();

  info_.clear();
}

/**
 * A Important report that is compatable with the Casal2 R package
 */
void Logging::FlushImportant() {
  std::scoped_lock l(lock_);
  if (important_.size() == 0) {
    return;
  }

  cout << "*important[messages_encountered]\n";
  cout << "Values " << REPORT_R_STRING_VECTOR << REPORT_EOL;
  for (unsigned i = 0; i < important_.size(); ++i) cout << important_[i] << REPORT_EOL;
  cout << REPORT_END << REPORT_EOL << REPORT_EOL;
  cout.flush();

  important_.clear();
}

/**
 * A warning report that is compatable with the Casal2 R package
 */
void Logging::FlushWarnings() {
  std::scoped_lock l(lock_);
  if (warnings_.size() == 0) {
    return;
  }

  // unsigned to_print = warnings_.size() > 10 ? 10 : warnings_.size();

  cout << "*warnings[messages_encountered]" << REPORT_EOL;
  cout << "Values " << REPORT_R_STRING_VECTOR << REPORT_EOL;
  for (unsigned i = 0; i < warnings_.size(); ++i) cout << warnings_[i] << REPORT_EOL;
  cout << REPORT_END << REPORT_EOL << REPORT_EOL;
  cout.flush();

  warnings_.clear();
}

} /* namespace niwa */
