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

#include "Utilities/Exception.h"

// Namespaces
namespace isam {
namespace utilities {

using std::vector;


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
    values[i] = isam::utilities::ToLowercase(values[i]);
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
  vector<string> true_values  = { "t", "true" };
  vector<string> false_values = { "f", "false" };

  string value = ToLowercase(arg);

  if (std::find(true_values.begin(), true_values.end(), value) != true_values.end()) {
    result = true;
    return true;
  }

  if (std::find(false_values.begin(), false_values.end(), value) != false_values.end()) {
    result = false;
    return true;
  }

  return false;
};




} /* namespace utilities */
} /* namespace isam */


#endif /* TO_H_ */
