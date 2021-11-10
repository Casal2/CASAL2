/**
 * @file Record.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 14/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is a single record that is being logged
 * too within the code. This object is created by the LOG()
 * macros and then passed to the logger to print it out if necessary
 */
#ifndef SOURCE_LOGGING_RECORD_H_
#define SOURCE_LOGGING_RECORD_H_

// headers
#include <sstream>

//#include "ParameterList/Parameter.h"

// namespaces
namespace niwa {
namespace logger {

using std::string;

// enums
enum class Severity { kInvalid, kTrace, kFinest, kFine, kMedium, kInfo, kImportant, kVerifyWarning, kWarning, kError, kFatal, kCodeError };

/**
 * Class definition
 */
class Record {
public:
  Record(Severity s, string file, string func, unsigned line) : log_severity_(s), file_name_(file), function_(func), line_number_(line){};
  virtual ~Record() = default;
  void BuildMessage();
  bool Flush();

  // accessors
  std::ostringstream& stream() { return stream_; }
  const std::string&  message() const { return message_; }
  Severity            severity() const { return log_severity_; }

private:
  bool               flush_ = false;
  string             message_;
  Severity           log_severity_;
  string             file_name_;
  string             function_;
  unsigned           line_number_;
  std::ostringstream stream_;
};

} /* namespace logger */
} /* namespace niwa */

#endif /* SOURCE_LOGGING_RECORD_H_ */
