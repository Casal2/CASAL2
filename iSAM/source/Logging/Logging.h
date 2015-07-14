/**
 * @file Logging.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 14/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is responsible for handling instances of the logging::Record
 * and printing them out if required
 */
#ifndef SOURCE_LOGGING_LOGGING_H_
#define SOURCE_LOGGING_LOGGING_H_

// headers
#include <iostream>
#include <vector>
#include <string>

#include "Logging/Record.h"

// namespaces
namespace niwa {

using std::cout;
using std::cerr;
using std::endl;

/**
 * Class definition
 */
class Logging {
public:
  // methods
  virtual                     ~Logging() = default;
  static Logging&             Instance();
  void                        Flush(niwa::logger::Record& record);
  void                        FlushErrors();
  void                        FlushWarnings();

  // accessors
  std::vector<std::string>&  warnings() { return warnings_; }
  std::vector<std::string>&  errors() { return errors_; }

private:
  // methods
  Logging();

  // members
  logger::Severity            current_log_level_ = logger::Severity::kWarning;
  std::vector<std::string>    warnings_;
  std::vector<std::string>    errors_;
};

} /* namespace niwa */

// Logging Macros
/**
 * These are our macro definitions for logging and exiting the application in the event
 * of an error.
 *
 * If we're building with TEST defined then we'll blank all of them except LOG_ERROR which
 * will throw an exception
 */
#define LOG_TRACE() for(logger::Record r(logger::Severity::kTrace, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r));
#define LOG_FINEST() for(logger::Record r(logger::Severity::kFinest, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r)) r.stream()
#define LOG_FINE() for(logger::Record r(logger::Severity::kFine, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r)) r.stream()
#define LOG_MEDIUM() for(logger::Record r(logger::Severity::kMedium, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r)) r.stream()
#define LOG_WARNING() for(logger::Record r(logger::Severity::kWarning, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r)) r.stream()
#define LOG_ERROR() for(logger::Record r(logger::Severity::kError, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r)) r.stream()
#define LOG_ERROR_P(parameter) for(logger::Record r(logger::Severity::kError, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r)) r.stream() << this->parameters_.location(parameter)
#define LOG_FATAL() for(logger::Record r(logger::Severity::kFatal, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r)) r.stream()
#define LOG_FATAL_P(parameter) for(logger::Record r(logger::Severity::kFatal, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r)) r.stream() << this->parameters_.location(parameter)
#define LOG_CODE_ERROR() for(logger::Record r(logger::Severity::kCodeError, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r)) r.stream()

#endif /* SOURCE_LOGGING_LOGGING_H_ */
