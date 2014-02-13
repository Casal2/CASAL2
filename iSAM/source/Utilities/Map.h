/**
 * @file Map.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 8/01/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file contains some useful code for building or dealing with maps
 */
#ifndef UTILITIES_MAP_H_
#define UTILITIES_MAP_H_

// headers
#include <vector>
#include <map>

#include "Types.h"

// namespaces
namespace isam {
namespace utilities {

using std::vector;
using std::map;

/**
 * Build a map from 2 vectors where the first vector
 * is the key and the second vector is the value
 *
 * @param key The keys to use
 * @param value The values to use
 * @return a Map of keys and values
 */
inline map<unsigned, Double> MapCreate(const vector<unsigned>& key, const vector<Double>& value) {
  map<unsigned, Double> result;

  for (unsigned i = 0; i < key.size(); ++i)
    result[key[i]] = value[i];

  return result;
}

} /* namespace utilities */
} /* namespace isam */

#endif /* MAP_H_ */
