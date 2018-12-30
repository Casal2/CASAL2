/**
 * @file Record.cpp
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 14/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "Record.h"

#include <sstream>
#include <iostream>

// namespaces
namespace niwa {
namespace logger {

using std::ostringstream;
using std::cout;
using std::endl;


/**
 * This method will return false on the first call,
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
 *
 */
void Record::BuildMessage() {

  /**
   * Build our log message for printing to the screen
   */
  ostringstream o;
  switch(log_severity_) {
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
  case Severity::kWarning:
    o << "[WARNING] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
    break;
  case Severity::kError:
    o << "[ERROR] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
//    o << "********************************************************************************\n";
//    o << "********                         ERROR                                  ********\n";
//    o << "********************************************************************************\n";
//    o << "Source File: " << file_name_ << "(line: " << line_number_ << ")\n";
//    o << "Source Method: " << function_ << "\n";
//    o << "\n";
//    o << "Message:\n";
//    o << stream_.str() << "\n\n";
    break;
  case Severity::kFatal:
#ifdef TESTMODE
    o << "[FATAL] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
#else
    o << "\n";
    o << "********************************************************************************\n";
    o << "********                  USER CONFIGURATION ERROR                      ********\n";
    o << "********              THE ERROR SHOULD BE DESCRIBED BELOW               ********\n";
    o << "********************************************************************************\n";
    o << "Source File: " << file_name_ << "(line: " << line_number_ << ")\n";
    o << "Source Method: " << function_ << "\n";
    o << "Message:\n";
    o << stream_.str() << "\n\n";
#endif
    break;
  case Severity::kCodeError:
#ifdef TESTMODE
    o << "[CODE_ERROR] " << file_name_ << "(line: " << line_number_ << "): " << stream_.str() << "\n";
#else
    o << "\n";
    o << "********************************************************************************\n";
    o << "********                CODE ERROR - CASAL2 is FORCE QUITTING           ********\n";
    o << "********************************************************************************\n";
    o << "Source File: " << file_name_ << "(line: " << line_number_ << ")\n";
    o << "Source Method: " << function_ << "\n";
    o << "\n";
    o << "Message:\n";
    o << stream_.str() << "\n";
    o << "NOTE: This error is a bug in the software and can be corrected by a developer\n";
    o << "Please contact the application developers at casal2@niwa.co.nz to have this issue fixed\n\n";
#endif
    break;
  }

  message_ = o.str();
}

} /* namespace logger */
} /* namespace niwa */
