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
    case Severity::kImportant:
#ifdef DEBUG
      o << "[IMPORTANT] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      o << " Source File: " << file_name_ << "(line: " << line_number_ << ")\n";
      o << " Source Method: " << function_ << "\n";
#endif
      o << "[IMPORTANT] " << stream_.str() << endl;
      break;
    case Severity::kInfo:
#ifdef DEBUG
      o << "[MESSAGE] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      o << " Source File: " << file_name_ << "(line: " << line_number_ << ")\n";
      o << " Source Method: " << function_ << "\n";
#endif
      o << "[INFO] " << stream_.str() << endl;
      break;
    case Severity::kVerifyWarning:
#ifdef DEBUG
      o << "[VERIFY_WARNING] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      o << " Source File: " << file_name_ << "(line: " << line_number_ << ")\n";
      o << " Source Method: " << function_ << "\n";
#endif
      o << "[VERIFY_WARNING] " << stream_.str() << endl;
      break;
    case Severity::kWarning:
#ifdef DEBUG
      o << "[WARNING] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      o << " Source File: " << file_name_ << "(line: " << line_number_ << ")\n";
      o << " Source Method: " << function_ << "\n";
#endif
      o << "[WARNING] " << stream_.str() << endl;
      break;
    case Severity::kError:
#ifdef DEBUG
      o << "[ERROR] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      o << " Source File: " << file_name_ << "(line: " << line_number_ << ")\n";
      o << " Source Method: " << function_ << "\n";
#endif
      o << "[ERROR] " << stream_.str() << endl;
      break;
    case Severity::kFatal:
#ifdef DEBUG
      o << "[FATAL] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      o << " Source File: " << file_name_ << "(line: " << line_number_ << ")\n";
      o << " Source Method: " << function_ << "\n";
#endif
      o << "[FATAL_ERROR] " << stream_.str() << endl;
      break;
    case Severity::kCodeError:
#ifdef DEBUG
      o << "[CODE_ERROR] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
      o << " Source File: " << file_name_ << "(line: " << line_number_ << ")\n";
      o << " Source Method: " << function_ << "\n";
#endif
      o << "[CODE_ERROR] A critical error has occurred in the Casal2 source code. ";
      o << "Please contact the Casal2 Development Team and report this event\n\n";
      o << stream_.str() << endl;
      break;
  }

  message_ = o.str();
}

} /* namespace logger */
} /* namespace niwa */
