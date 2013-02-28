/*
 * DoubleCompare.h
 *
 *  Created on: 20/12/2012
 *      Author: Admin
 */

#ifndef DOUBLECOMPARE_H_
#define DOUBLECOMPARE_H_

#include "Utilities/Types.h"

// Namespaces
namespace isam {
namespace utilities {
namespace doublecompare {

// Defines
#define ONE  1
#define TRUE_ZERO 0
#define ZERO 1e-15
#define DELTA 1e-11

inline bool IsZero(const double &value) { return (value < ZERO && value > -ZERO); }
inline bool IsTrueZero(const double &value) { return (value < TRUE_ZERO && value > -TRUE_ZERO); }
inline bool IsOne(const double &value) { return ( ((value-ONE) < ZERO) && ((value-ONE) > -ZERO) ); }
inline bool IsEqual(double A, double B) { return ( ((A-B) < ZERO) && ((A-B) > -ZERO) ); }

inline isam::utilities::Double ZeroFun(double x) {
  if (x >= ZERO)
    return x;

  return ZERO / (2 - (x / ZERO));
}

} /* namespace doublecompare */
} /* namespace utilities */
} /* namespace isam */



//  static bool     isZero(double A) { return ( (A < ZERO) && (A > -ZERO) ); }
//  static bool     isTrueZero(double A) { return ( (A < TRUE_ZERO) && (A > -TRUE_ZERO) ); }
//  static bool     isNonNegative(double A) { return ( 0.0 <= A ); }
//  static bool     isPositive(double A) { return ( 0.0 < A ); }
//  static bool     isEqual(double A, double B) { return ( ((A-B) < ZERO) && ((A-B) > -ZERO) ); }
//  static bool     isBetween(double A, double B, double C) {
//    return ( ((A-B) > -ZERO) && ((A-C) < ZERO) );
//  }


#endif /* DOUBLECOMPARE_H_ */
