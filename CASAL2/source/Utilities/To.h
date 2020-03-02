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
 * Some nice and short conversion functions
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

#include "Logging/Logging.h"
#include "Utilities/Exception.h"
#include "Utilities/PartitionType.h"
#include "Utilities/Types.h"
#include "Translations/Translations.h"

// Namespaces
namespace niwa {
namespace utilities {

using std::vector;
using std::string;

/**
 * This method converts a string into a lowercase version of it
 *
 * @param arg The string to convert
 * @return a lowercase version of the string
 */
inline string ToLowercase(const ::std::string arg) {
  string value = arg;
  for (char &c : value)
    c = tolower(c);

  return value;
};

/**
 * This method will convert an entire vector of strings to lowercase.
 *
 * @param values The string vector to convert
 */
inline void ToLowercase(vector<string> &values) {

  for (unsigned i = 0; i < values.size(); ++i) {
    values[i] = niwa::utilities::ToLowercase(values[i]);
  }
}

/**
 * This is a shortened version of the method used
 * to call boost::lexical_cast<Target>(source).
 *
 * @param arg The string to convert
 * @param result The variable to store the conversion
 * @return true (for conversion successful) or false
 */
template<typename Target>
bool To(const ::std::string arg, Target &result) {
  result = Target();
  try {
    result = boost::lexical_cast<Target>(arg);
  } catch (...) {
    return false;
  }
  return true;
};

/**
 * This method will iterate through a vector and attempt to convert every
 * element in to the target type. Any invalid strings will be returned
 * in a vector
 *
 * @param arg The string vector to convert
 * @param result The vector variable to store the conversions
 * @return string vector of invalid strings
 */
template<typename Target>
vector<string> To(const vector<string>& source, vector<Target>& result) {
  vector<string> invalids;

  for (string value : source) {
    try {
      result.push_back(boost::lexical_cast<Target>(value));
    } catch (...) {
      invalids.push_back(value);
    }
  }

  return invalids;
}

/**
 * This is a specialisation for handling unsigned ints that are put in as negative
 *
 * @param arg The string to convert to an unsigned integer
 * @param result The variable to store the conversion
 * @return true (for successful conversion) or false
 */
template <>
inline bool To(const ::std::string arg, unsigned &result) {
  result = (unsigned)0;
  try {
    int temp = boost::lexical_cast<int>(arg);
    if (temp < 0)
      return false;

    result = (unsigned)temp;
  } catch (...) {
    return false;
  }

  return true;
}

/**
 * This is a specialization that handles boolean types
 *
 * @param arg The string to check for valid boolean type
 * @param result The variable to store the conversion
 * @return true/false. Exception on failure
 */
template<>
inline bool To(const ::std::string arg, bool &result) {
  result = false;
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
}

/**
 * This is a specialization that handles PartitionType types
 *
 * @param arg The string to check for valid PartitionType type
 * @param result The variable to store the conversion
 * @return true/false. Exception on failure
 */
template<>
inline bool To(const ::std::string arg, PartitionType &result) {
  result = PartitionType::kInvalid;
  string value = ToLowercase(arg);

  if (value == PARAM_AGE)
    result = PartitionType::kAge;
  else if (value == PARAM_LENGTH)
    result = PartitionType::kLength;
//  else if (value == PARAM_HYBRID)
//    result = PartitionType::kHybrid;
  else if (value == PARAM_MODEL)
    result = PartitionType::kModel;

  bool success = result != PartitionType::kInvalid;

  return success;
}

/**
 * This is a specialisation for handling unsigned ints that are put in as negative
 *
 * @param arg The variable to convert
 * @param result The variable to store the conversion
 * @result true (for successful conversion) or false
 */
template <typename Source>
inline bool To(const Source& arg, unsigned &result) {
  result = (unsigned)0;
  try {
    int temp = boost::lexical_cast<int>(arg);
    if (temp < 0)
      return false;

    result = (unsigned)temp;
  } catch (...) {
    return false;
  }
  return true;
}

/**
 * This is a method for converting from one type to another type.
 *
 * @param arg The variable to convert
 * @param result The variable to store the conversion
 * @result true (for successful conversion) or false
 */
template<typename Source, typename Target>
bool To(const Source& arg, Target& result) {
  result = Target();
  try {
    result = boost::lexical_cast<Target>(arg);
  } catch (...) {
    return false;
  }
  return true;
}

/**
 * This is a method for converting from one type to another type.
 *
 * @param arg The variable to convert
 * @return The converted variable. Exception on failure
 */
template<typename Source, typename Target>
Target ToInline(const Source arg) {
  Target result = Target();
  try {
    result = boost::lexical_cast<Target>(arg);

  } catch (...) {
    LOG_CODE_ERROR() << "Failed to do an inline conversion from " << arg;
  }
  return result;
}

} /* namespace utilities */
} /* namespace niwa */


#endif /* TO_H_ */
