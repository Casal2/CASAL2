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

#include "Logging/Record.h"

// namespaces
namespace niwa {

/**
 * Class definition
 */
class Logging {
public:
  // methods
  Logging() { };
  virtual                     ~Logging() = default;
  static Logging&             Instance();
  void                        Flush(niwa::logger::Record& record);
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
#ifdef TESTMODE
#define LOG_TRACE() { };
#define LOG_INFO(value) { };
#define LOG_WARNING(value) { };
#define LOG_ERROR_P(value) {\
  ostringstream o;\
  o << "[ERROR] - An error has been encountered that prevents the program from continuing\n"\
    << "Source: " << __FILE__ << ":" << __FUNCTION__ << " (Line: " << __LINE__ << ")\n"\
    << "Error: " << value;\
  cout << o.str() << endl;\
  throw std::string(o.str()); }

#define LOG_CODE_ERROR(value) {\
    ostringstream o;\
    o << "\n\n\n";\
    o << "[ERROR] - A coding error has been encountered that prevents the program from continuing\n"\
      << "[ERROR] - This error requires developer intervention to correct. Please contact a developer\n"\
      << "Source: " << __FILE__ << ":" << __FUNCTION__ << " (Line: " << __LINE__ << ")\n"\
      << "Error: " << value;\
    cout << o.str() << endl;\
    throw std::string(o.str()); }

#else

#define LOG_TRACE() for(logger::Record r(logger::Severity::kError, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r));
#define LOG_FINEST() for(logger::Record r(logger::Severity::kFinest, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r)) r.stream()
#define LOG_FINE() for(logger::Record r(logger::Severity::kFine, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r)) r.stream()
#define LOG_WARNING() for(logger::Record r(logger::Severity::kWarning, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r)) r.stream()
#define LOG_ERROR() for(logger::Record r(logger::Severity::kError, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r)) r.stream()
#define LOG_ERROR_P(parameter) for(logger::Record r(logger::Severity::kError, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r)) r.stream() << this->parameters_.location(parameter)
#define LOG_CODE_ERROR() for(logger::Record r(logger::Severity::kCodeError, __FILE__, __FUNCTION__, __LINE__); !r.Flush(); Logging::Instance().Flush(r)) r.stream()

#endif

#endif /* SOURCE_LOGGING_LOGGING_H_ */
