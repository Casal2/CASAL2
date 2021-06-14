/**
 * @file Math.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 25/03/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
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
#include <map>
#include <vector>

// C Headers for quick double to string n back conversion
#include <stdio.h>  /* printf, NULL */
#include <stdlib.h> /* strtod */

#include "../Logging/Logging.h"
#include "../Utilities/Distribution.h"
#include "../Utilities/Types.h"

// Namespaces
namespace niwa {
namespace utilities {
namespace math {

using niwa::utilities::Double;
using std::map;
using std::vector;

// Const Expressions
constexpr double ONE       = 1.0;
constexpr double TRUE_ZERO = 0.0;
constexpr double ZERO      = 1e-15;
constexpr double CLOSE     = 1e-5;
constexpr double DELTA     = 1e-11;
constexpr double PI        = 3.1415926535897932384626433832795028;

inline bool IsZero(const Double& value) {
  return (value < ZERO && value > -ZERO);
}

// inline bool IsZero(const Double &value) { return (value < ZERO && value > -ZERO); }
// inline bool IsInfinite(const Double &value) { return (isinf(value));}
inline bool IsTrueZero(const Double& value) {
  return (value < TRUE_ZERO && value > -TRUE_ZERO);
}
inline bool IsOne(const Double& value) {
  return (((value - ONE) < ZERO) && ((value - ONE) > -ZERO));
}
inline bool IsEqual(Double A, Double B) {
  return (((A - B) < ZERO) && ((A - B) > -ZERO));
}
inline bool IsBasicallyEqual(Double A, Double B) {
  return (((A - B) < CLOSE) && ((A - B) > -CLOSE));
}

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

#ifdef USE_AUTODIFF

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

/**
 * LnGamma
 */
inline Double LnGamma(Double t) {
  Double x, y, tmp, ser;
  Double cof[6] = {76.18009172947146, -86.50532032941677, 24.01409824083091, -1.231739572450155, 0.1208650973866179e-2, -0.5395239384953e-5};
  y = x = t;
  tmp   = x + 5.5 - (x + 0.5) * log(x + 5.5);
  ser   = 1.000000000190015;
  for (unsigned j = 0; j <= 5; j++) {
    y = y + 1.0;
    ser += (cof[j] / y);
  }
  return (log(2.5066282746310005 * ser / x) - tmp);
}

/**
 * LnFactorial
 */
inline Double LnFactorial(Double t) {
  return niwa::utilities::math::LnGamma(t + 1.0);
}
/*
 *
 */

/**
 * dnorm: return the pdf for the normal
 */
inline Double dnorm(const Double& x, const Double& mu, const Double& sigma) {
  Double z = 1.0 / (sigma * sqrt(2.0 * PI)) * exp(-((x - mu) * (x - mu)) / (2.0 * sigma * sigma));
  return (z);
}

//************************************
// These are distributional functions taken from CASAL, some will wont to be updated
//************************************
/*
 * dlognorm: return the pdf for the lognormal
 */
inline Double dlognorm(const Double& x, const Double& mu = 0.0, const Double& sigma = 1.0) {
  // Parameterised by the mean and standard deviation of the (normal) distribution
  //  of log(x), NOT by those of the (lognormal) distribution of x.
  if (x <= 0)
    return 0.0;
  else
    return dnorm(log(x), mu, sigma);
}

/**
 * pnorm: return the cdf for the normal
 */
inline Double pnorm(const Double& x, const Double& mu = 0.0, const Double& sigma = 1.0) {
  // Abramowitz & Stegun eqn 26.2.18
  // Equations: z = fabs((x-mu)/sigma);
  //            p = 1-0.5*pow((1+0.196854*z+0.115194*z*z+0.000344*z*z*z+0.019527*z*z*z*z),-4);
  //            if (x<mu) p=1-p;
  Double z = fabs((x - mu) / sigma);
  Double p = 1 - 0.5 * pow((1 + 0.196854 * z + 0.115194 * z * z + 0.000344 * z * z * z + 0.019527 * z * z * z * z), -4);
  if (x < mu)
    p = 1 - p;
  return (p);
}

/**
 * pnorm2: return the cdf for the normal that may be more expensive (computationally) but is a better approximation.
 */
inline Double pnorm2(const Double& x, const Double& mu = 0.0, const Double& sigma = 1.0) {
  // Ian Doonan's code, A better approximation of the normal CDF as there is no closed form
  Double norm, ttt, p;
  Double z  = fabs((x - mu) / sigma);
  Double tt = 1.0 / (1.0 + 0.2316419 * z);
  norm      = (1.0 / sqrt(2.0 * PI)) * exp(-0.5 * z * z);
  ttt       = tt;
  p         = 0.319381530 * ttt;
  ttt       = ttt * tt;
  p         = p - 0.356563782 * ttt;
  ttt       = ttt * tt;
  p         = p + 1.781477937 * ttt;
  ttt       = ttt * tt;
  p         = p - 1.821255978 * ttt;
  ttt       = ttt * tt;
  p         = p + 1.330274429 * ttt;
  p *= norm;
  p = 1 - p;
  if (x < mu)
    p = 1 - p;
  return (p);
}

/**
 * plognorm: return the cdf for the lognormal
 */
inline Double plognorm(const Double& x, const Double& mu, const Double& sigma) {
  // Parameterised by the mean and standard deviation of the (normal) distribution
  //  of log(x), NOT by those of the (lognormal) distribution of x.
  if (x <= 0)
    return 0.0;
  else
    return pnorm(log(x), mu, sigma);
}

/**
 * plognorm2: return the cdf for the lognormal
 */
inline Double plognorm2(const Double& x, const Double& mu, const Double& sigma) {
  // Parameterised by the mean and standard deviation of the (normal) distribution
  //  of log(x), NOT by those of the (lognormal) distribution of x.
  if (x <= 0)
    return 0.0;
  else
    return pnorm2(log(x), mu, sigma);
}

// Distribution: this is taken from CASAL, will be useful for Samik and his length structured stuff.
// CASAL source code development.h
// Probability distribution of a random variable over classes.
// distribution(class_mins,plus_group,dist,mean,stdev)[i]
// is the probability that a random variable with distribution 'dist', 'mean' and 'stdev'
// falls between class_mins[i] and class_mins[i+1]
// (unless i = 0 and plus_group!=0, in which case
// distribution(...)[i] is the probability that the random variable exceeds class_mins[i]).
// TODO change dist from string -> enum Think about moving enum Distribution from AgeLength to model as it will be used everywhere in the code base.
//
// This function uses an approximation: P(X is more than 5 std.devs away from its mean) = 0.
//  Almost true for the normal distribution, but may be problematic for more skewed distributions.
/**
 * distribution: uses the normal CDF functions from CASAL
 */
inline vector<Double> distribution(const vector<Double>& class_mins, bool plus_group = false, const Distribution& dist = Distribution::kNormal, const Double& mean = 0.0,
                                   const Double& stdev = 1.0) {
  int            n_bins = class_mins.size() - (plus_group ? 0 : 1);
  vector<Double> result(n_bins, 0.0);
  Double         so_far = 0;
  Double         mu = 0, sigma = 0;
  // Adjust mu and sigma for lognormal
  if (dist == Distribution::kLogNormal) {
    sigma = sqrt(log(1 + pow(stdev / mean, 2)));
    mu    = log(mean) - sigma * sigma / 2;
  }
  if (class_mins[0] < mean - 5 * stdev) {
    so_far = 0;
  } else {
    if (dist == Distribution::kNormal) {
      so_far = pnorm(class_mins[0], mean, stdev);
    } else if (dist == Distribution::kLogNormal) {
      so_far = plognorm(class_mins[0], mu, sigma);
    } else
      LOG_CODE_ERROR() << "unknown distribution supplies, this should be caught earlier '";
  }

  int c;
  for (c = 0; c < (n_bins - 1); c++) {
    if (class_mins[c + 1] > mean + 5 * stdev) {
      result[c] = 1 - so_far;
      so_far    = 1;
    } else if (class_mins[c + 1] < mean - 5 * stdev) {
      result[c] = 0;
      so_far    = 0;
    } else {
      if (dist == Distribution::kNormal) {
        result[c] = pnorm(class_mins[c + 1], mean, stdev) - so_far;
      } else if (dist == Distribution::kLogNormal) {
        result[c] = plognorm(class_mins[c + 1], mu, sigma) - so_far;
      }
      so_far += result[c];
    }
    if (result[c] < 0 || result[c] != result[c]) {
      LOG_CODE_ERROR() << "bad result in distribution, parsed " << result[c];
    }
  }
  LOG_TRACE();

  c = n_bins - 1;
  if (plus_group) {
    result[c] = 1 - so_far;
  } else {
    if (class_mins[c + 1] > mean + 5 * stdev) {
      result[c] = 1 - so_far;
    } else {
      if (dist == Distribution::kNormal) {
        result[c] = pnorm(class_mins[c + 1], mean, stdev) - so_far;
      } else if (dist == Distribution::kLogNormal) {
        result[c] = plognorm(class_mins[c + 1], mu, sigma) - so_far;
      }
    }
    if (result[c] < 0 || result[c] != result[c]) {
      LOG_CODE_ERROR() << "bad result in distribution, parsed " << result[c];
    }
  }
  return result;
}

/**
 * distribution2: uses the updated/more accurate normal CDF functions
 */
inline vector<Double> distribution2(const vector<Double>& class_mins, bool plus_group = false, const Distribution& dist = Distribution::kNormal, const Double& mean = 0.0,
                                    const Double& stdev = 1.0) {
  int            n_bins = class_mins.size() - (plus_group ? 0 : 1);
  vector<Double> result(n_bins, 0.0);
  Double         so_far = 0;
  Double         mu = 0, sigma = 0;
  // Adjust mu and sigma for lognormal
  if (dist == Distribution::kLogNormal) {
    sigma = sqrt(log(1 + pow(stdev / mean, 2)));
    mu    = log(mean) - sigma * sigma / 2;
  }
  if (class_mins[0] < mean - 5 * stdev) {
    so_far = 0;
  } else {
    if (dist == Distribution::kNormal) {
      so_far = pnorm2(class_mins[0], mean, stdev);
    } else if (dist == Distribution::kLogNormal) {
      so_far = plognorm2(class_mins[0], mu, sigma);
    } else
      LOG_CODE_ERROR() << "unknown distribution";
  }

  int c;
  for (c = 0; c < (n_bins - 1); c++) {
    if (class_mins[c + 1] > mean + 5 * stdev) {
      result[c] = 1 - so_far;
      so_far    = 1;
    } else if (class_mins[c + 1] < mean - 5 * stdev) {
      result[c] = 0;
      so_far    = 0;
    } else {
      if (dist == Distribution::kNormal) {
        result[c] = pnorm2(class_mins[c + 1], mean, stdev) - so_far;
      } else if (dist == Distribution::kLogNormal) {
        result[c] = plognorm2(class_mins[c + 1], mu, sigma) - so_far;
      }
      so_far += result[c];
    }
    if (result[c] < 0 || result[c] != result[c]) {
      LOG_CODE_ERROR() << "bad result in distribution, parsed " << result[c];
    }
  }
  LOG_TRACE();

  c = n_bins - 1;
  if (plus_group) {
    result[c] = 1 - so_far;
  } else {
    if (class_mins[c + 1] > mean + 5 * stdev) {
      result[c] = 1 - so_far;
    } else {
      if (dist == Distribution::kNormal) {
        result[c] = pnorm2(class_mins[c + 1], mean, stdev) - so_far;
      } else if (dist == Distribution::kLogNormal) {
        result[c] = plognorm2(class_mins[c + 1], mu, sigma) - so_far;
      }
    }
    if (result[c] < 0 || result[c] != result[c]) {
      LOG_CODE_ERROR() << "bad result in distribution, parsed " << result[c];
    }
  }
  return result;
}

/**
 * @brief Scale a value using an tan transformation
 *
 * @param value value to scale
 * @param min lower bound
 * @param max upper bound
 * @return double
 */
inline Double scale(Double value, Double min, Double max) {
  Double scaled = tan(((value - min) / (max - min) - 0.5) * PI);
  return scaled;
}

/**
 * @brief Unscale all values in the vector inplace
 *
 * @param target
 */
inline void scale_vector(vector<Double>& target, const vector<Double>& lower_bounds, const vector<Double>& upper_bounds) {
  for (unsigned i = 0; i < target.size(); ++i) {
    target[i] = scale(target[i], lower_bounds[i], upper_bounds[i]);
  }
}

/**
 * @brief Unscale value back from tan to natural space
 *
 * @param value
 * @param min
 * @param max
 * @return double
 */
inline Double unscale(Double value, Double min, Double max, bool truncate_value = false) {
  Double unscaled = ((atan(value) / PI) + 0.5) * (max - min) + min;
  if (truncate_value) {
    char s1[128] = {0};
    snprintf(s1, 128, "%.14f", unscaled);
    unscaled = strtod(s1, NULL);
  }
  return unscaled;
}

/**
 * @brief Unscale all values in the vector inplace
 *
 * @param target
 */
inline void unscale_vector(vector<Double>& target, const vector<Double>& lower_bounds, const vector<Double>& upper_bounds, bool truncate_values = false) {
  for (unsigned i = 0; i < target.size(); ++i) {
    target[i] = unscale(target[i], lower_bounds[i], upper_bounds[i], truncate_values);
  }
}

//**********************************************************************
//    General math utilities
//**********************************************************************
// Return the mean for a vector
inline Double mean(const vector<Double>& Values) {
  Double mu    = 0.0;
  Double total = 0.0;
  for (const auto& value : Values) total += value;
  Double n = Values.size();
  mu       = total / n;
  return mu;
}

// Return the mean for an unsigned map
inline Double mean(const map<unsigned, Double>& Values) {
  Double mu    = 0.0;
  Double total = 0.0;
  for (const auto& value : Values) total += value.second;
  Double n = Values.size();
  mu       = total / n;
  return mu;
}

// Return the Variance for a vector
inline Double Var(const vector<Double>& Values) {
  Double mean_    = math::mean(Values);
  Double variance = 0;
  for (const auto& value : Values) variance += (value - mean_) * (value - mean_);
  Double n   = Values.size();
  Double var = variance / (n - 1.0);
  return var;
}

// Return the Variance for an unsigned map
inline Double Var(const map<unsigned, Double>& Values) {
  Double mean_    = math::mean(Values);
  Double variance = 0;
  for (const auto& value : Values) variance += (value.second - mean_) * (value.second - mean_);
  Double n   = Values.size();
  Double var = variance / (n - 1.0);
  return var;
}

// Return the Standard Deviation for a vector
inline Double std_dev(const vector<Double>& Values) {
  Double sd;
  sd = sqrt(math::Var(Values));
  return sd;
}

// Return the Standard Deviation for an unsigned map
inline Double std_dev(const map<unsigned, Double>& Values) {
  Double sd;
  sd = sqrt(math::Var(Values));
  return sd;
}

// Return the maximum value for a vector
inline Double Max(const vector<Double>& Values) {
  Double   max  = 0.0;
  unsigned iter = 1;
  for (auto value : Values) {
    if (iter == 1)
      max = value;
    else if (max < value)
      max = value;
    ++iter;
  }
  return max;
}

} /* namespace math */
} /* namespace utilities */
} /* namespace niwa */
#endif /* UTILITIES_MATH_H_ */
