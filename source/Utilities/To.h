/**
 * @file To.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 18/09/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file gives us some nice and short conversion routines
 * using the boost::lexical_cast<> with some specialisations
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef TO_H_
#define TO_H_

// Headers
#include <string>
#include <boost/lexical_cast.hpp>

#include "Exception.h"

namespace iSAM {
namespace Utilities {

/**
 * This is a shortened version of the method used
 * to call boost::lexical_cast<Target>(source);.
 */
template<typename Target, typename Source>
Target To(const Source& arg) {
  return boost::lexical_cast<Target>(arg);
};

/**
 * This is one of our specializations that handles
 * boolean types
 */
template<>
inline bool To(const ::std::string& arg) {

  string value = arg;
  for (unsigned i = 0; i < value.length(); ++i)
    value[i] = toupper(value[i]);

  if (arg == "TRUE" || arg == "T")
    return true;
  else if (arg == "FALSE" || arg == "F")
    return false;

  THROW_EXCEPTION("Invalid boolean conversion from string " + arg);
};

/**
 * This method converts a string into a lowercase version of it
 */
inline string ToLowercase(const ::std::string &arg) {
  string return_value = arg;
  for (unsigned i = 0; i < return_value.size(); ++i)
    return_value[i] = tolower(return_value[i]);

  return return_value;
};


} /* namespace Utilities */
} /* namespace iSAM */


#endif /* TO_H_ */
