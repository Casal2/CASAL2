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

#include "../Utilities/Types.h"

// namespaces
namespace niwa {
namespace utilities {
using std::vector;
using std::pair;

/**
 * This class is responsible for providing us with a specialised container that allows
 * the storage and retrieval of data in an "order by insertion" method.
 *
 * This means that no matter what order you put the data in to the map in it'll be
 * retrieved in the same order.
 *
 */
template<typename _Key, typename _Tp>
class OrderedMap {
public:
	OrderedMap() = default;
	~OrderedMap() = default;

	/**
	 * This behaves like the operator on the std::map.
	 * If the object exists we return it, if not we create
	 * a default entry for it.
	 */
	_Tp& operator[] (const _Key& search_key) {
		for (auto& [ key, value ] : storage_)
			if (key == search_key)
				return value;

		std::pair<_Key, _Tp> new_value = std::pair<_Key, _Tp>(search_key, _Tp());
		storage_.push_back(new_value);
		return storage_[storage_.size() - 1].second;
	}

	/**
	 * This behaves like the std::map->find() method
	 * in that it's a const search that does not
	 * add a value to the storage_
	 */
	const auto find(const _Key& search_key) const {
		for (auto iter = storage_.begin(); iter != storage_.end(); ++iter)
			if (iter->first == search_key)
				return iter;

		return storage_.end();
	}

	/**
	 * Return size
	 */
	auto size() const { return storage_.size(); }

	/**
	 * Handlers for begin() and end() so that we can use range
	 * based for loops
	 */
	auto begin() { return storage_.begin(); }
	auto end() { return storage_.end(); }

private:
	vector<std::pair<_Key, _Tp>> storage_;
};

/**
 * Namespace for our map methods
 */
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
  static std::map<unsigned, Double> create(const std::vector<unsigned>& key, const std::vector<Double>& value) {
    std::map<unsigned, Double> result;

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
  static std::map<std::string, Double> create(const std::vector<std::string>& key, const std::vector<Double>& value) {
    std::map<std::string, Double> result;

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
  static std::map<unsigned, Double> create(const std::vector<unsigned>& key, const Double& value) {
    std::map<unsigned, Double> result;

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
