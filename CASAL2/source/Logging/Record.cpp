/**
 * @file Record.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 14/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 */

// headers
#include "Record.h"

#include <iostream>
#include <sstream>

// namespaces
namespace niwa {
namespace logger {

using std::cout;
using std::endl;
using std::ostringstream;

/**
 * This method returns false on the first call,
 * then true to ensure the log record is flushed after it
 * has finished being used.
 */
bool Record::Flush() {
  if (!flush_) {
    flush_ = true;
    return false;
  }

  return true;
}

/**
 * This method builds the logging message
 */
void Record::BuildMessage() {
  /**
   * Build our log message for printing to the screen
   */
  ostringstream o;
  switch (log_severity_) {
    case Severity::kInvalid:
      o << "\n";
      o << "[INVALID] The logging system has a bug where the record Severity is not being flagged correctly\n";
      o << "Source File: " << file_name_ << "(line: " << line_number_ << ")\n";
      o << "Source Method: " << function_ << "\n";
      o << "\n";
      o << "Message:\n";
      o << stream_.str() << "\n";
      break;
    case Severity::kTrace:
      o << "[TRACE] " << file_name_ << "(line: " << line_number_ << ") - Method: " << function_ << "\n";
      break;
    case Severity::kFinest:
      o << "[FINEST] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      break;
    case Severity::kFine:
      o << "[FINE] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      break;
    case Severity::kMedium:
      o << "[MEDIUM] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      break;
    case Severity::kCoarse:
      o << "[COARSE] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      break;
    case Severity::kInfo:
      o << "[INFORMATION] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      break;
    case Severity::kWarning:
      o << "[WARNING] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      break;
    case Severity::kError:
      o << "[ERROR] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      break;
    case Severity::kFatal:
#ifdef TESTMODE
      o << "[FATAL] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      o << "Source File: " << file_name_ << "(line: " << line_number_ << ")\n";
      o << "Source Method: " << function_ << "\n";
#else
      o << "\n";
      o << "Casal2 has found syntax errors in the input configuration file:\n";
#endif
      o << "\n";
      o << stream_.str() << "\n\n";
      break;
    case Severity::kCodeError:
#ifdef TESTMODE
      o << "[CODE_ERROR] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      o << "Source File: " << file_name_ << "(line: " << line_number_ << ")\n";
      o << "Source Method: " << function_ << "\n";
#else
      o << "\n";
      o << "CODE ERROR: A critical error has occurred:\n";
      o << "\n";
      o << stream_.str() << "\n";
      o << "This is a critical error in the Casal2 code base.\n";
      o << "Please contact the Casal2 Development Team at casal2@niwa.co.nz to report this  error\n\n";
#endif
      break;
  }

  message_ = o.str();
}

} /* namespace logger */
} /* namespace niwa */
