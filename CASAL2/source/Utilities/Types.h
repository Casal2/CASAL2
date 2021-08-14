/**
 * @file Types.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/10/2012
 * @section LICENSE
 *
 * Copyright NIWA Science �2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file contains conditional types that we'll use in Casal2 instead
 * of the default types to give us more flexibility.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef TYPES_H_
#define TYPES_H_

#include <cstdlib>
#include <memory>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif
#include <iostream>
#include <string>
#include <vector>

#ifdef USE_ADOLC
#include <adolc/adolc.h>

adub acos(const badouble&);
adub asin(const badouble&);
adub atan(const badouble&);
adub ceil(const badouble&);
adub cos(const badouble&);
adub cosh(const badouble&);
adub exp(const badouble&);
adub fabs(const badouble&);
adub floor(const badouble&);
adub log(const badouble&);
adub log10(const badouble&);
adub sin(const badouble&);
adub sinh(const badouble&);
adub sqrt(const badouble&);
adub tan(const badouble&);
adub tanh(const badouble&);
#endif

/**
 * @brief Attempted to integrate with the Advanced Template Library
 * https://github.com/msupernaw/ATL
 *
 * Problem is that it doesn't have explicit type conversion or operator
 * overloading for streams (<< and >>) so it's not compatible with Boost et al.
 *
 * things like return asin(atl_double - 1) will fail because no int to double conversion
 */
// #ifdef USE_ATL
// #include <ATL/ATL.hpp>
// #endif

#ifdef USE_BETADIFF
#include <ADOLC18/SRC/adolc.h>
#endif

#ifdef USE_CPPAD
#include <cppad/cppad.hpp>
#include <cppad/ipopt/solve.hpp>
#endif

// Namespaces
namespace niwa {
namespace utilities {

// using std::istream;
// using std::ostream;

/**
 * double conditional depending on if we're using auto differentiation or not
 *
 * The AS_DOUBLE macro is used to convert from the auto-differentation type
 * to a standard double. This is used for two reasons.
 * 1. To convert to a double for piping into a stream when an operator overload doesn't exist
 * 2. To force break the chain to store the values for reporting out
 *
 * We're not worried about performance penalties for either as most scenarios for
 * doing this will be to debug and this isn't on by default.
 */
#ifdef USE_ADOLC
typedef adouble Double;
#define AS_DOUBLE(x) x.value()
#endif

#ifdef USE_BETADIFF
typedef adouble Double;
#define AS_DOUBLE(x) x.toDouble()
#endif

#ifdef USE_CPPAD
typedef CppAD::AD<double> Double;
// per https://coin-or.github.io/CppAD/doc/ad_output.htm
// use this (CppAD::AD<base> has '<<'' defined)
#define AS_DOUBLE(x) CppAD::Value(x)
#endif

// #ifdef USE_ATL
// typedef atl::Variable<double> Double;
// #define AS_DOUBLE(x) ((double)x)

// ostream& operator<<(ostream& out, const atl::Variable<double>& c) {
//   out << (double)c;
//   return out;
// }

// istream& operator>>(istream& in, atl::Variable<double>& c) {
//   double d = (double)c;
//   in >> d;
//   return in;
// }
// #endif

#ifndef USE_AUTODIFF
#ifdef Double
#error "Compiler flags are incorrect. USE_AUTODIFF has not been defined, but an auto-differentiation system has been"
#endif

#define AS_DOUBLE(x) x
typedef double Double;
#endif

typedef std::vector<Double>                                        Vector1;
typedef std::vector<std::vector<Double>>                           Vector2;
typedef std::vector<std::vector<std::vector<Double>>>              Vector3;
typedef std::vector<std::vector<std::vector<std::vector<Double>>>> Vector4;

inline void allocate_vector3(Vector3* target, unsigned x_size, unsigned y_size, unsigned z_size) {
  target->resize(x_size);
  for (unsigned i = 0; i < x_size; ++i) {
    (*target)[i].resize(y_size);
    for (unsigned j = 0; j < y_size; ++j) (*target)[i][j].resize(z_size);
  }
}

/**
 * This code is used to demangle the typeid(x).name information
 */
inline std::string demangle(const char* name) {
#ifndef _MSC_VER
  int status = -1;  // some arbitrary value to eliminate the compiler warning

  char*       realname;
  std::string val = "";
  realname        = abi::__cxa_demangle(name, 0, 0, &status);
  val             = realname;
  free(realname);

  val = val == "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >" ? "string" : val;
  val = val.length() > 38 && val.substr(0, 38) == "std::vector<std::__cxx11::basic_string" ? "vector<string>" : val;
  val = val == "std::vector<double, std::allocator<double> >" ? "vector<double>" : val;
  val = val == "std::vector<unsigned int, std::allocator<unsigned int> >" ? "vector<unsigned int>" : val;
  return (status == 0) ? val : name;
#else
  return name;
#endif
}

} /* namespace utilities */
} /* namespace niwa */

#endif /* TYPES_H_ */
