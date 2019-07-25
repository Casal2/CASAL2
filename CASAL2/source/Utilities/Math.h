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

#include "Utilities/DoubleCompare.h"
#include "Utilities/Types.h"
#include "Utilities/Distribution.h"

#define PI 3.14159265358979

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
/*
 *
*/

/**
 * dnorm: return the pdf for the normal
 */
inline Double dnorm(const Double& x, const Double& mu, const Double& sigma) {
  Double z = 1 / (sigma * sqrt(2 * PI)) * exp(-((x - mu) * (x - mu))/(2 * sigma * sigma));
  return(z);
}

//************************************
// These are distributional functions taken from CASAL, some will wont to be updated
//************************************
/*
 * dlognorm: return the pdf for the log-normal
 */
inline Double dlognorm(const Double& x, const Double& mu = 0.0, const Double& sigma = 1.0) {
  // Parameterised by the mean and standard deviation of the (normal) distribution
  //  of log(x), NOT by those of the (lognormal) distribution of x.
  if (x <= 0)
    return 0.0;
  else
    return dnorm(log(x),mu,sigma);
}

/**
 * pnorm: return the cdf for the normal
 */
inline Double pnorm(const Double& x, const Double& mu = 0.0, const Double& sigma = 1.0) {
  // Abramowitz & Stegun eqn 26.2.18
  // Equations: z = fabs((x-mu)/sigma);
  //            p = 1-0.5*pow((1+0.196854*z+0.115194*z*z+0.000344*z*z*z+0.019527*z*z*z*z),-4);
  //            if (x<mu) p=1-p;
  Double z = fabs((x - mu)/sigma);
  Double p = 1 - 0.5*pow((1+0.196854*z+0.115194*z*z+0.000344*z*z*z+0.019527*z*z*z*z),-4);
  if (x < mu)
    p = 1 - p;
  return(p);
}

/**
 * pnorm2: return the cdf for the normal that may be more expensive (computationally) but is a better approximation.
 */
inline Double pnorm2(const Double& x, const Double& mu = 0.0, const Double& sigma = 1.0) {
  // Ian Doonan's code, A better approximation of the normal CDF as there is no closed form
  Double norm, ttt, p;
  Double z = fabs((x - mu)/sigma);
  Double tt = 1.0 / (1.0 + 0.2316419 * z);
  norm = 1.0 / sqrt(2.0 * PI) * exp(-0.5 * z * z);
  ttt = tt;
  p = 0.319381530 * ttt;
  ttt = ttt * tt;
  p = p - 0.356563782 * ttt;
  ttt = ttt * tt;
  p = p + 1.781477937 * ttt;
  ttt = ttt * tt;
  p = p - 1.821255978 * ttt;
  ttt = ttt * tt;
  p = p + 1.330274429 * ttt;
  p *= norm;
  if (x < mu)
    p = 1-p;
  return(p);
}

/**
 * plnorm: return the cdf for the normal
 */
inline Double plognorm(const Double& x, const Double& mu, const Double& sigma) {
  // Parameterised by the mean and standard deviation of the (normal) distribution
  //  of log(x), NOT by those of the (lognormal) distribution of x.
  if (x <= 0)
    return 0.0;
  else
    return pnorm(log(x),mu,sigma);
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
// We use an approximation: P(X is more than 5 std.devs away from its mean) = 0.
//  Almost true for the normal distribution, but may be problematic if you use something more skewed.
inline vector<Double> distribution(const vector<Double>& class_mins, bool plus_group = 0, const Distribution& dist = Distribution::kNormal, const Double& mean = 0.0, const Double& stdev = 1.0) {
  int n_bins = class_mins.size() - (plus_group ? 0 : 1);
  vector<Double> result(n_bins, 0.0);
  Double so_far = 0;
  Double mu = 0, sigma = 0;
  // Adjust mu and sigma for lognormal
  if (dist == Distribution::kLogNormal){
    sigma = sqrt(log(1+pow(stdev/mean,2)));
    mu = log(mean) - sigma*sigma/2;
  }
  if (class_mins[0] < mean - 5 * stdev) {
    so_far = 0;
  } else {
    if (dist == Distribution::kNormal){
      so_far = pnorm(class_mins[0],mean,stdev);
    } else if (dist == Distribution::kLogNormal){
      so_far = plognorm(class_mins[0],mu,sigma);
    } else
      LOG_CODE_ERROR() << "unknown distribution supplies, this should be caught earlier '";
  }

  int c;
  for (c = 0; c < (n_bins - 1); c++) {
    if (class_mins[c + 1] > mean + 5 * stdev){
      result[c] = 1-so_far;
      so_far = 1;
    } else if (class_mins[c + 1] < mean - 5 * stdev){
      result[c] = 0;
      so_far = 0;
    } else {
      if (dist == Distribution::kNormal){
        result[c] = pnorm(class_mins[c + 1], mean ,stdev) - so_far;
      } else if (dist == Distribution::kLogNormal){
        result[c] = plognorm(class_mins[c + 1], mu, sigma) - so_far;
      }
      so_far += result[c];
    }
    if (result[c] < 0 || result[c]!=result[c]) {
      LOG_CODE_ERROR() << "bad result in distribution, got " << result[c];
    }
  }
  LOG_TRACE();

  c = n_bins - 1;
  if (plus_group){
    result[c] = 1 - so_far;
  } else {
    if (class_mins[c + 1] > mean + 5 * stdev){
      result[c] = 1 - so_far;
    } else {
      if (dist == Distribution::kNormal){
        result[c] = pnorm(class_mins[c + 1], mean, stdev) - so_far;
      } else if (dist == Distribution::kLogNormal){
        result[c] = plognorm(class_mins[c + 1], mu, sigma) - so_far;
      }
    }
    if (result[c] < 0 || result[c] != result[c]){
      LOG_CODE_ERROR() << "bad result in distribution, got " << result[c];
    }
  }
  return result;
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
//    General math utilities
//**********************************************************************
// Return the mean for a vector
inline Double mean(const vector<Double>& Values){
  Double mu = 0.0;
  Double total = 0.0;
  for (const auto& value : Values)
    total += value;
  Double n = AS_DOUBLE(Values.size();
  mu = total / n);
  return mu;
}

// Return the mean for an unsigned map
inline Double mean(const map<unsigned, Double>& Values){
  Double mu = 0.0;
  Double total = 0.0;
  for (const auto& value : Values)
    total += value.second;
  Double n = Values.size();
  mu = total / n;
  return mu;
}

// Return the Variance for a vector
inline Double Var(const vector<Double>& Values){
  Double mean_ = math::mean(Values);
  Double variance = 0;
  for (const auto& value : Values)
    variance += (value - mean_) * (value - mean_);
  Double n = Values.size();
  Double var = variance / (n - 1.0);
  return var;
}

// Return the Variance for an unsigned map
inline Double Var(const map<unsigned, Double>& Values){
  Double mean_ = math::mean(Values);
  Double variance = 0;
  for (const auto& value : Values)
    variance += (value.second - mean_) * (value.second - mean_);
  Double n = Values.size();
  Double var = variance / (n - 1.0);
  return var;
}

// Return the Standard Deviation for a vector
inline Double std_dev(const vector<Double>& Values){
  Double sd;
  sd = sqrt(math::Var(Values));
  return sd;
}

// Return the Standard Deviation for an unsigned map
inline Double std_dev(const map<unsigned, Double>& Values){
  Double sd;
  sd = sqrt(math::Var(Values));
  return sd;
}

// Return the maximum value for a vector
inline Double Max(const vector<Double>& Values){
  Double max = 0.0;
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
