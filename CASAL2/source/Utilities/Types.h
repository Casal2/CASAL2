/**
 * @file Types.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 22/10/2012
 * @section LICENSE
 *
 * Copyright NIWA Science ©2012 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This file contains conditional types that we'll use in CASAL2 instead
 * of the default types to give us more flexibility.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef TYPES_H_
#define TYPES_H_

#include <cstdlib>
#include <memory>
#include <cxxabi.h>
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

#ifdef USE_BETADIFF
#include <ADOLC18/SRC/adolc.h>
#endif

#ifdef USE_CPPAD
# include <cppad/cppad.hpp>
#include <cppad/ipopt/solve.hpp>
#endif

// Namespaces
namespace niwa {
namespace utilities {

/**
 * double conditional depending on if we're using auto differentiation or not
 */
#ifdef USE_ADOLC
typedef adouble Double;
#define AS_DOUBLE(x) x.value()
#define AS_VALUE(x) AS_DOUBLE(x)

#ifndef ADOLC_OPERATOR_DEF
#define ADOLC_OPERATOR_DEF
// need to redefine due to definition in ThirdParty/adolc/ADOL-C-2.5.2/ADOL-C/src/adouble.cpp
inline std::ostream& operator << ( std::ostream& out, const Double& y ) {
  return out << AS_VALUE(y) ;
}
#endif  /* ADOLC_OPERATOR_DEF */

#endif

#ifdef USE_BETADIFF
typedef adouble Double;
#define AS_DOUBLE(x) x.toDouble()
#define AS_VALUE(x) AS_DOUBLE(x)
#endif

#ifdef USE_CPPAD
typedef CppAD::AD<double> Double;
// per https://coin-or.github.io/CppAD/doc/ad_output.htm
// use this (CppAD::AD<base> has '<<'' defined)
#define AS_DOUBLE(x) x
#define AS_VALUE(x) Value(x)
#endif

#ifndef USE_AUTODIFF
#ifdef Double
#error "Compiler flags are incorrect. USE_AUTODIFF has not been defined, but an auto-differentiation system has been"
#endif

#define AS_DOUBLE(x) x
#define AS_VALUE(x) AS_DOUBLE(x)
typedef double Double;
#endif

typedef std::vector<Double> Vector1;
typedef std::vector<std::vector<Double>> Vector2;
typedef std::vector<std::vector<std::vector<Double>>> Vector3;
typedef std::vector<std::vector<std::vector<std::vector<Double>>>> Vector4;

inline void allocate_vector3(Vector3* target, unsigned x_size, unsigned y_size, unsigned z_size) {
  target->resize(x_size);
  for (unsigned i = 0; i < x_size; ++i) {
    (*target)[i].resize(y_size);
    for (unsigned j = 0; j < y_size; ++j)
      (*target)[i][j].resize(z_size);
  }
}

/**
 * This code is used to demangle the typeid(x).name information
 */
inline std::string demangle(const char* name) {
  int status = -1; // some arbitrary value to eliminate the compiler warning

  char   *realname;
  std::string val = "";
  realname = abi::__cxa_demangle(name, 0, 0, &status);
  val = realname;
  free(realname);

  val = val == "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >" ? "string" : val;
  val = val.length() > 38 && val.substr(0, 38) == "std::vector<std::__cxx11::basic_string" ? "vector<string>" : val;
  val = val == "std::vector<double, std::allocator<double> >" ? "vector<double>" : val;
  val = val == "std::vector<unsigned int, std::allocator<unsigned int> >" ? "vector<unsigned int>" : val;
  return (status==0) ? val : name ;
}


} /* namespace utilities */
} /* namespace niwa */



#endif /* TYPES_H_ */
