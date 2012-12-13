/**
 * @file Logging.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 12/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class provides some basic logging functionality to the application.
 * While this class can be accessed directly the majority of actions will be
 * accessed through macros that provide ostringstream output.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef LOGGING_H_
#define LOGGING_H_

// Headers
#include <string>
#include <sstream>

// Using
using std::string;
using std::ostringstream;

namespace isam {
namespace utilities {

/**
 * These are our macro definitions for logging and exiting the application in the event
 * of an error.
 *
 * If we're building with TEST defined then we'll blank all of them except LOG_ERROR which
 * will throw an exception
 */

// Macros
#define LOG_TRACE() {\
  ostringstream o;\
  o << "--> Trace: " << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__;\
  isam::utilities::Logging::Instance().Log(o.str()); }

#define LOG_INFO(value) {\
  ostringstream o;\
  o << "[I] " << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << " = " << value;\
  isam::utilities::Logging::Instance().Log(o.str()); }

#define LOG_WARNING(value) {\
  ostringstream o;\
  o << "[W] " << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << " = " << value;\
  isam::utilities::Logging::Instance().Log(o.str()); }

#define LOG_ERROR(value) {\
  ostringstream o;\
  o << "\n\n\n";\
  o << "[ERROR] - An error has been encountered that prevents the program from continuing\n"\
    << "Source: " << __FILE__ << ":" << __FUNCTION__ << " (Line: " << __LINE__ << ")\n"\
    << "Error: " << value;\
  isam::utilities::Logging::Instance().ForceLog(o.str()); \
  exit(-1); }

#define LOG_CODE_ERROR(value) {\
    ostringstream o;\
    o << "\n\n\n";\
    o << "[ERROR] - A coding error has been encountered that prevents the program from continuing\n"\
      << "[ERROR] - This error requires developer intervention to correct. Please contact a developer\n"\
      << "Source: " << __FILE__ << ":" << __FUNCTION__ << " (Line: " << __LINE__ << ")\n"\
      << "Error: " << value;\
    isam::utilities::Logging::Instance().ForceLog(o.str()); \
    exit(-1); }
/**
 * Class Definition: Logging
 */
class Logging {
public:
  // Methods
  static Logging&             Instance();
  virtual                     ~Logging() = default;
  void                        Log(const string &value);
  void                        ForceLog(const string &value);

private:
  // Methods
  Logging() = default;
};

} /* namespace utilities */
} /* namespace isam */
#endif /* LOGGING_H_ */
