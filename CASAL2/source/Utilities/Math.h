/**
 * @file Math.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 25/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * The time class represents a moment of time.
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef UTILITIES_MATH_H_
#define UTILITIES_MATH_H_

// Headers
#include <cmath>

#include "Utilities/DoubleCompare.h"
#include "Utilities/Types.h"

// Namespaces
namespace niwa {
namespace utilities {
namespace math {

namespace dc = niwa::utilities::doublecompare;
using niwa::utilities::Double;

/**
 * LnGamma
 */
inline Double LnGamma(Double t) {
  Double x, y, tmp, ser;
  Double cof[6] = {76.18009172947146,-86.50532032941677,24.01409824083091,-1.231739572450155,0.1208650973866179e-2,-0.5395239384953e-5};
  y = x = t;
  tmp = x + 5.5 - (x + 0.5) * log(x + 5.5);
  ser = 1.000000000190015;
  for (unsigned j = 0; j <= 5; j++) {
    y = y + 1.0;
    ser += (cof[j] / y);
  }
  return(log(2.5066282746310005 * ser / x) - tmp);
}

/**
 * LnFactorial
 */
inline Double LnFactorial(Double t) {
  return niwa::utilities::math::LnGamma(t + 1.0);
}


//**********************************************************************
// void Engine::condassign( double &res, const double &cond, const double &arg1, const double &arg2 ) {
// Conditional Assignment
//**********************************************************************
inline void cond_assign(Double &res, const Double &cond, const Double &arg1, const Double &arg2) {
  res = (cond) > 0 ? arg1 : arg2;
}

//**********************************************************************
// void Engine::condassign( double &res, const double &cond, const double &arg)
// Conditional Assignment
//**********************************************************************
inline void cond_assign(Double &res, const Double &cond, const Double &arg) {
  res = (cond) > 0 ? arg : res;
}

/**
 * double Engine::boundpin(double y, double fmin, double fmax)
 * Boundary Pin
 */

inline Double scale_value(Double value, Double min, Double max) {
  if (dc::IsEqual(value, min))
    return -1;
  else if (dc::IsEqual(value, max))
    return 1;

  return asin(2 * (value - min) / (max - min) - 1) / 1.57079633;
}

/**
 *
 */
inline Double unscale_value(const Double& value, Double& penalty, Double min, Double max) {
  // courtesy of AUTODIF - modified to correct error -
  // penalty on values outside [-1,1] multiplied by 100 as of 14/1/02.
  Double t = 0.0;
  Double y = 0.0;

  t = min + (max - min) * (sin(value * 1.57079633) + 1) / 2;
  cond_assign(y, -.9999 - value, (value + .9999) * (value + .9999), 0);
  penalty += y;
  cond_assign(y, value - .9999, (value - .9999) * (value - .9999), 0);
  penalty += y;
  cond_assign(y, -1 - value, 1e5 * (value + 1) * (value + 1), 0);
  penalty += y;
  cond_assign(y, value - 1, 1e5 * (value - 1) * (value - 1), 0);
  penalty += y;

  return (t);
}

//**********************************************************************
//    Convert units from unit to kgs
//**********************************************************************
inline Double convert_units_to_kgs(Double Value, string unit) {
  Double converted_value = 0.0;
  if (unit == PARAM_TONNES)
    converted_value = Value * 1000;
  else if (unit == PARAM_KGS)
    converted_value = Value;
  else if (unit == PARAM_GRAMS) {
    converted_value = Value / 1000;
  } else {
    LOG_FATAL() << "The unit type: " << unit << " is not expected, Values expected are kgs, tonnes, grammes";
  }
  return converted_value;
}


} /* namespace math */
} /* namespace utilities */
} /* namespace niwa */
#endif /* UTILITIES_MATH_H_ */
