/**
 * @file Vector.h
 * @author Scott Rasmussen (scott@zaita.com)
 * @brief Some helper functions for manipulating vectors as math vectors
 * @version 0.1
 * @date 2021-05-13
 *
 * @copyright Copyright (c) 2021
 *
 */

// headers
#include <iostream>
#include <vector>

// namespaces
namespace niwa::utilities {

// using
using std::cout;
using std::endl;
using std::vector;

/**
 * @brief Add the second vector to the first vector
 *
 * @param target Vector to have values added to
 * @param values Vector to take values from to add to target
 */
inline void Vector_Add(vector<double>& target, const vector<double>& values) {
  for (unsigned i = 0; i < target.size(); ++i) {
    target[i] += values[i];
  }
}

/**
 * @brief Multiple the target vector by the scale_by vector.
 * This is effectively scaling the target vector
 *
 * @param target Vector to modify
 * @param scale_by Vector to take values from to multiple against target
 */
inline void Vector_Scale(vector<double>& target, double scale_by) {
  for (unsigned i = 0; i < target.size(); ++i) {
    target[i] *= scale_by;
  }
}

/*
Generic function to find an element in vector and also its position.
It returns a pair of bool & int i.e.
bool : Represents if element is present in vector or not.
int : Represents the index of element in vector if its found else -1
*/
template < typename T>
std::pair<bool, int > findInVector(const std::vector<T>  & vecOfElements, const T  & element) {
    std::pair<bool, int > result;
    // Find given element in vector
    auto it = std::find(vecOfElements.begin(), vecOfElements.end(), element);
    if (it != vecOfElements.end())
    {
        result.second = distance(vecOfElements.begin(), it);
        result.first = true;
    }
    else
    {
        result.first = false;
        result.second = -1;
    }
    return result;
}

/**
 * @brief Print the contents of a vector to a single line for debugging.
 * We use this when building new algorithms to make life a bit easier
 *
 * @param label Print this at the start of the line before the data
 * @param data vector of information we wan to print to stdout
 */
inline void Vector_Debug(const char* label, vector<double>& data) {
  bool debug = false;
  if (debug) {
    cout << label;
    for (unsigned i = 0; i < data.size() - 1; ++i) {
      cout << data[i] << ", ";
    }
    cout << data[data.size() - 1] << endl;
  }
}

}  // namespace niwa::utilities