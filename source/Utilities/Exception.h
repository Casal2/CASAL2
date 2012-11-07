/**
 * @file Exception.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
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
#include <string>
#include <sstream>
#include <iostream>
#include <exception>

// Namespaces
using std::string;
using std::ostringstream;
using std::stringstream;
using std::cout;
using std::endl;
using std::exception;

namespace isam {
namespace utilities {

// Macro
#define THROW_EXCEPTION(X) { ostringstream o; o << __FILE__ << ":" << __LINE__ << endl << "## " << X << endl; throw string(o.str()); }
#define RETHROW_EXCEPTION(X) { ostringstream o; o << __FILE__ << ":" << __LINE__ << endl << X; throw string(o.str()); }
#define RETHROW_LABELLED_EXCEPTION(X) { ostringstream o; o << __FILE__ << ":" << __LINE__ << " (" << pParameterList->getString(PARAM_LABEL, "Unknown") << ") " << endl << X; throw string(o.str()); }

// iSAM Exception Class


} /* namespace utilities */
} /* namespace isam */


#endif /* EXCEPTION_ */
