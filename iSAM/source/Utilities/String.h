/**
 * @file String.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 16/11/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file contains some common string routines for NIWA applications
 * that make it easy to do manipulations
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef UTILITIES_STRING_H_
#define UTILITIES_STRING_H_

// Headers
#include <string>
#include <vector>
#include <sstream>


#include "Translations/Translations.h"

// Namespaces
namespace niwa {
namespace utilities {
namespace strings {

using std::string;
using std::vector;

std::string find_invalid_characters(const std::string& test_string);

vector<std::string> explode(const std::string& source);

template<typename T>
std::string join(const std::vector<T>& source, const string separator = "") {

  std::ostringstream result;
  for (unsigned i = 0; i < source.size() - 1; ++i)
    result  << source[i] << separator;
  result << source[source.size() - 1];

  return result.str();
}

} /* namespace strings */
} /* namespace utilities */
} /* namespace niwa */

#endif /* STRING_H_ */
