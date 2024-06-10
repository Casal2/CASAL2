/**
 * @file Exception.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright Casal2 Project 2024 - https://github.com/Casal2/
 *
 * @section DESCRIPTION
 *
 * This header contains some handy macros for throwing exceptions.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef EXCEPTION_
#define EXCEPTION_

// Headers
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

// Namespaces
using std::cout;
using std::endl;
using std::ostringstream;
using std::string;
using std::stringstream;

namespace niwa {
namespace utilities {

class StopException : public std::exception {
public:
  StopException()  = default;
  ~StopException() = default;
};

//#define THROW_EXCEPTION(X) { ostringstream o; o << __FILE__ << ":" << __LINE__ << endl << "## " << X << endl; throw string(o.str()); }
//#define RETHROW_EXCEPTION(X) { ostringstream o; o << __FILE__ << ":" << __LINE__ << endl << X; throw string(o.str()); }

} /* namespace utilities */
} /* namespace niwa */

#endif /* EXCEPTION_ */
