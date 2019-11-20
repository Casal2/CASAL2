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
namespace niwa {
namespace utilities {
namespace doublecompare {

using niwa::utilities::Double;

// Defines
#define ONE  1.0
#define TRUE_ZERO 0.0
#define ZERO 1e-15
#define CLOSE 1e-5
#define DELTA 1e-11

inline bool IsZero(const Double &value) { return (value < ZERO && value > -ZERO); }
//inline bool IsInfinite(const Double &value) { return (isinf(value));}
inline bool IsTrueZero(const Double &value) { return (value < TRUE_ZERO && value > -TRUE_ZERO); }
inline bool IsOne(const Double &value) { return ( ((value-ONE) < ZERO) && ((value-ONE) > -ZERO) ); }
inline bool IsEqual(Double A, Double B) { return ( ((A-B) < ZERO) && ((A-B) > -ZERO) ); }
inline bool IsBasicallyEqual(Double A, Double B) { return ( ((A-B) < CLOSE) && ((A-B) > -CLOSE) ); }

inline niwa::utilities::Double ZeroFun(Double x) {
  if (x >= ZERO)
    return x;

  return ZERO / (2.0 - (x / ZERO));
}

inline niwa::utilities::Double ZeroFun(Double x, Double delta) {
  if (x >= delta)
    return x;

  return delta / (2.0 - (x / delta));
}


// mix of Double and double
inline bool IsEqual(Double A, double B) { return ( ((AS_VALUE(A)-B) < ZERO) && ((AS_VALUE(A)-B) > -ZERO) ); }
inline bool IsBasicallyEqual(Double A, double B) { return ( ((AS_VALUE(A)-B) < CLOSE) && ((AS_VALUE(A)-B) > -CLOSE) ); }

inline niwa::utilities::Double ZeroFun(Double x, double delta) {
  if (x >= delta)
    return x;

  return delta / (2.0 - (x / delta));
}


#ifdef USE_AUTODIFF
inline bool IsZero(const double &value) { return (value < ZERO && value > -ZERO); }
//inline bool IsInfinite(const double &value) { return (isinf(value));}
inline bool IsTrueZero(const double &value) { return (value < TRUE_ZERO && value > -TRUE_ZERO); }
inline bool IsOne(const double &value) { return ( ((value-ONE) < ZERO) && ((value-ONE) > -ZERO) ); }
inline bool IsEqual(double A, double B) { return ( ((A-B) < ZERO) && ((A-B) > -ZERO) ); }
inline bool IsBasicallyEqual(double A, double B) { return ( ((A-B) < CLOSE) && ((A-B) > -CLOSE) ); }

inline double ZeroFun(double x, double delta) {
  if (x >= delta)
    return x;

  return delta / (2.0 - (x / delta));
}

inline double ZeroFun(double x) {
  if (x >= ZERO)
    return x;

  return ZERO / (2.0 - (x / ZERO));
}
#endif

} /* namespace doublecompare */
} /* namespace utilities */
} /* namespace niwa */





#endif /* DOUBLECOMPARE_H_ */
