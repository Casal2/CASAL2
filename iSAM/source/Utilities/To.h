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
#include <vector>
#include <boost/lexical_cast.hpp>

#include "Utilities/Logging/Logging.h"
#include "Utilities/Exception.h"


// Namespaces
namespace niwa {
namespace utilities {

using std::vector;
using std::string;


/**
 * This method converts a string into a lowercase version of it
 *
 * @param Copy of the string to lowercase
 * @return a lowercase version of the string
 */
inline string ToLowercase(const ::std::string arg) {
  string value = arg;
  for (char &c : value)
    c = tolower(c);

  return value;
};

/**
 * This method will lowercase an entire vector of strings.
 *
 * @param values The vector containing strings to lowercase
 */
inline void ToLowercase(vector<string> &values) {

  for (unsigned i = 0; i < values.size(); ++i) {
    values[i] = niwa::utilities::ToLowercase(values[i]);
  }
}

/**
 * This is a shortened version of the method used
 * to call boost::lexical_cast<Target>(source);.
 */
template<typename Target>
bool To(const ::std::string arg, Target &result) {
  try {
    result = boost::lexical_cast<Target>(arg);
  } catch (...) {
    return false;
  }
  return true;
};

/**
 * This is one of our specializations that handles
 * boolean types
 *
 * @param arg The arguement to check for valid boolean type
 * return true/false. Exception on failure
 */
template<>
inline bool To(const ::std::string arg, bool &result) {

  try {
    result = boost::lexical_cast<bool>(arg);
    return true;

  } catch (...) {
    vector<string> true_values  = { "t", "true", "yes", "y" };
    vector<string> false_values = { "f", "false", "no", "n" };

    string value = ToLowercase(arg);

    if (std::find(true_values.begin(), true_values.end(), value) != true_values.end()) {
      result = true;
      return true;
    }

    if (std::find(false_values.begin(), false_values.end(), value) != false_values.end()) {
      result = false;
      return true;
    }
  }

  return false;
};

/**
 * This is a method of converting from a known type to another
 * type. This method differs from above because we don't have
 * a hard-coded string source
 */
template<typename Source, typename Target>
bool To(const Source& arg, Target& result) {
  try {
    result = boost::lexical_cast<Target>(arg);
  } catch (...) {
    return false;
  }
  return true;
}

/**
 * This method will do an inline conversion and return
 * the result instead of loading a parameter with it.
 */
template<typename Source, typename Target>
Target ToInline(const Source arg) {
  Target result;
  try {
    result = boost::lexical_cast<Target>(arg);

  } catch (...) {
    LOG_CODE_ERROR("Failed to do an inline conversion from " << arg);
  }
  return result;
}

} /* namespace utilities */
} /* namespace niwa */


#endif /* TO_H_ */
