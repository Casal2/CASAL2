/**
 * @file Logging.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 14/04/2015
 * @section LICENSE
 *
 * Copyright NIWA Science �2015 - www.niwa.co.nz
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
#include <mutex>
#include <string>
#include <vector>

#include "../Logging/Record.h"

// namespaces
namespace niwa {

using std::cerr;
using std::cout;
using std::endl;

/**
 * Class definition
 */
class Logging {
public:
  // methods
  virtual ~Logging() = default;
  static Logging& Instance();
  void            SetLogLevel(const std::string& log_level);
  void            Flush(niwa::logger::Record& record);
  void            FlushErrors();
  void            FlushWarnings();
  void            FlushVerifies();
  void            FlushInfo();
  void            FlushImportant();

  // accessors
  std::vector<std::string>& info() { return info_; }
  std::vector<std::string>& important() { return important_; }
  std::vector<std::string>& warnings() { return warnings_; }
  std::vector<std::string>& errors() { return errors_; }
  std::vector<std::string>& verifies() { return verifies_; }

  // static members
  static logger::Severity current_log_level_;
  static std::mutex       lock_;

private:
  // methods
  Logging();

  // members
  std::vector<std::string> info_;
  std::vector<std::string> important_;
  std::vector<std::string> warnings_;
  std::vector<std::string> verifies_;
  std::vector<std::string> errors_;
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

#define LOG_IF(level) if (level >= Logging::current_log_level_)
#define LOG_FOR(level) for (logger::Record r(level, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r))
#define LOG_IF_FOR(level) LOG_IF(level) LOG_FOR(level)
#define LOG_IF_FOR_STREAM(level) LOG_IF_FOR(level) r.stream()
// Various levels of log level reporting
#define LOG_TRACE() LOG_IF_FOR(logger::Severity::kTrace);
#define LOG_FINEST() LOG_IF_FOR_STREAM(logger::Severity::kFinest)
#define LOG_FINE() LOG_IF_FOR_STREAM(logger::Severity::kFine)
#define LOG_MEDIUM() LOG_IF_FOR_STREAM(logger::Severity::kMedium)
// Information messages (not an error)
#define LOG_INFO() LOG_IF_FOR_STREAM(logger::Severity::kInfo)
// Important messages (not an error)
#define LOG_IMPORTANT() LOG_IF_FOR_STREAM(logger::Severity::kImportant)
// Verify messages (an error, except during unit tests) can be bypassed with --verifylevel warning or -V warning
#define LOG_VERIFY() LOG_IF_FOR_STREAM(logger::Severity::kVerify)
// Warning messages (not an error, except during test)
#define LOG_WARNING() LOG_IF_FOR_STREAM(logger::Severity::kWarning)
// Errors that do not cause an immediate stop, but are logged and reported all at once
#define LOG_ERROR() LOG_IF_FOR_STREAM(logger::Severity::kError)
#define LOG_ERROR_P(parameter) LOG_IF_FOR_STREAM(logger::Severity::kError) << this->parameters_.location(parameter)
// Errors that do cause an immediate stop and exit of the program
#define LOG_FATAL() LOG_FOR(logger::Severity::kFatal) r.stream()
#define LOG_FATAL_P(parameter) LOG_FOR(logger::Severity::kFatal) r.stream() << this->parameters_.location(parameter)
// Code errors that require a developer to fix
#define LOG_CODE_ERROR() LOG_FOR(logger::Severity::kCodeError) r.stream()

#endif /* SOURCE_LOGGING_LOGGING_H_ */
