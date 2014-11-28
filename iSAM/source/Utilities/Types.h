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
 * This file contains conditional types that we'll use in iSAM instead
 * of the default types to give us more flexibility.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef TYPES_H_
#define TYPES_H_

#include <cstdlib>
#include <memory>
#include <cxxabi.h>

#ifdef USE_ADOLC
#include <adolc/adouble.h>
#endif

#ifdef USE_BETADIFF
#include <ADOLC18/SRC/adolc.h>
#endif

#ifdef USE_CPPAD
# include <cppad/cppad.hpp>
#include <cppad/ipopt/solve.hpp>
#endif

// Namespaces
namespace isam {
namespace utilities {

/**
 * double conditional depending on if we're using auto differentiation or not
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
#define AS_DOUBLE(x) x.getValue()
#endif

#ifndef USE_AUTODIFF
#ifdef Double
#error "Compiler flags are incorrect. USE_AUTODIFF has not been defined, but an auto-differentiation system has been"
#endif

#define AS_DOUBLE(x) x
typedef double Double;
#endif


/**
 * This code is used to demangle the typeid(x).name information
 */
inline std::string demangle(const char* name) {

    int status = -4; // some arbitrary value to eliminate the compiler warning

    // enable c++11 by passing the flag -std=c++11 to g++
    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };

    return (status==0) ? res.get() : name ;
}


} /* namespace utilities */
} /* namespace isam */



#endif /* TYPES_H_ */
