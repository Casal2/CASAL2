/**
 * @file Distribution.h
 * @author C.Marsh
 * @github https://github.com/Craig44
 * @date 25/07/2019
 * @section LICENSE
 *
 * Copyright NIWA Science ©2019 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file contains the list of model distribution types
 */

#ifndef UTILITIES_DISTRIBUTION_H_
#define UTILITIES_DISTRIBUTION_H_

#include <string>

// Enumerated Types
enum class Distribution {
  kNone = 0,
  kNormal = 1,
  kLogNormal = 2,
};

inline Distribution operator&(Distribution a, Distribution b) {
  return static_cast<Distribution>(static_cast<int>(a) & static_cast<int>(b));
}

inline Distribution operator|(Distribution a, Distribution b) {
  return static_cast<Distribution>(static_cast<int>(a) | static_cast<int>(b));
}


inline Distribution operator||(Distribution a, Distribution b) {
  return static_cast<Distribution>(static_cast<int>(a) || static_cast<int>(b));
}

#endif /* UTILITIES_DISTRIBUTION_H_ */
