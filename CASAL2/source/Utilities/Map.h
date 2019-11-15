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
#include <set>
#include <string>

#include "Utilities/Types.h"

// namespaces
namespace niwa {
namespace utilities {

/**
 * This class is responsible for providing us with a specialised container that allows
 * the storage and retrieval of data in an "order by insertion" method.
 *
 * This means that no matter what order you put the data in to the map in it'll be
 * retrieved in the same order.
 *
 */
struct cmp_by_insertion {
  std::vector<std::string> current_keys_;

  bool operator() (std::string const &a, std::string const &b) {
    if (a == b)
      return false;

    unsigned a_index = current_keys_.size() + 10;
    unsigned b_index = current_keys_.size() + 10;

    for (unsigned i = 0; i < current_keys_.size(); ++i) {
      if (current_keys_[i] == a)
        a_index = i;
      if (current_keys_[i] == b)
        b_index = i;
    }

    if (a_index > current_keys_.size())
      current_keys_.push_back(a);
    if (b_index > current_keys_.size())
      current_keys_.push_back(b);

    return a_index <= b_index;
  }
};

template<typename _Key, typename _Tp>
class OrderedMap : public ::std::map<_Key, _Tp, cmp_by_insertion> { };

/**
 * Namespace for our map methods
 */
template<typename _Tp>
class Map {
public:
  /**
   * Build a map from 2 vectors where the first vector
   * is the key and the second vector is the value
   *
   * @param key The keys to use
   * @param value The values to use
   * @return a Map of keys and values
   */
  static std::map<unsigned, _Tp> create(const std::vector<unsigned>& key, const std::vector<_Tp>& value) {
    std::map<unsigned, _Tp> result;

    for (unsigned i = 0; i < key.size(); ++i)
      result[key[i]] = value[i];

    return result;
  }

  /**
   * Build a map from 2 vectors where the first vector
   * is the key and the second vector is the value
   *
   * @param key The keys to use
   * @param value The values to use
   * @return a Map of keys and values
   */
  static std::map<std::string, _Tp> create(const std::vector<std::string>& key, const std::vector<_Tp>& value) {
    std::map<std::string, _Tp> result;

    for (unsigned i = 0; i < key.size(); ++i)
      result[key[i]] = value[i];

    return result;
  }

  /**
   * Build a map from 1 vector of keys and a scalar where the first vector
   * is the key and the second value is set for all values of the key
   *
   * @param key The keys to use
   * @param value The values to use
   * @return a Map of keys and values
   */
  static std::map<unsigned, _Tp> create(const std::vector<unsigned>& key, const _Tp& value) {
    std::map<unsigned, _Tp> result;

    for (unsigned i = 0; i < key.size(); ++i)
      result[key[i]] = value;

    return result;
  }


}; // class

template<class A, class B, class C>
using map2D = std::map<A, std::map<B, C> >;

template<class A, class B, class C, class D>
using map3D = std::map<A, std::map<B, std::map<C, D> > >;

} /* namespace utilities */
} /* namespace niwa */

#endif /* MAP_H_ */
