/**
 * @file RandomNumberGenerator.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */

// Headers
#include "RandomNumberGenerator.h"

#include "../Model/Model.h"

// Namespaces
namespace niwa {
namespace utilities {

/**
 * Default Constructor
 */
RandomNumberGenerator::RandomNumberGenerator() {}

/**
 * Destructor
 */
RandomNumberGenerator::~RandomNumberGenerator() {}

/**
 * Singleton instance method
 * @return reference to singleton object
 */
RandomNumberGenerator& RandomNumberGenerator::Instance() {
  static RandomNumberGenerator instance;
  return instance;
}

/**
 * Reset the random number generator with a new seed
 *
 * @param new_seed The new seed
 */
void RandomNumberGenerator::Reset(unsigned new_seed) {
  LOG_MEDIUM() << "Reset RNG Seed: " << new_seed;
  generator_.seed(new_seed);
  seeds_.push_back(new_seed);
  sequence_.push_back(1);
}

/**
 * Generate a random uniform number between min and max
 *
 * @param min The smallest number to be generated (default 0.0)
 * @param max The largest number to be generated (default 1.0)
 * @return random number
 */
double RandomNumberGenerator::uniform(double min, double max) {
  boost::uniform_real<>                                             uniform(min, max);
  boost::variate_generator<boost::mt19937&, boost::uniform_real<> > generator(generator_, uniform);

  double value = generator();
  LOG_FINEST() << "rng.uniform(" << min << ", " << max << ") = " << value;

  sequence_.push_back(0);
  rng_uniform_values_.push_back(value);

  return value;
}

/**
 * Generate a normal random number
 *
 * @param mean (default 0.0)
 * @param sigma (default 1.0)
 * @return random number
 */
double RandomNumberGenerator::normal(double mean, double sigma) {
  boost::normal_distribution<>                                             normal(mean, sigma);
  boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > generator(generator_, normal);

  double value = generator();
  LOG_FINEST() << "rng.normal(" << mean << ", " << sigma << ") = " << value;
  sequence_.push_back(0);
  rng_normal_values_.push_back(value);
  return value;
}

/**
 * Generate a lognormal random number
 *
 * @param mean
 * @param cv
 * @return random number
 */
double RandomNumberGenerator::lognormal(double mean, double cv) {
  double log_sigma = sqrt(log(cv * cv + 1.0));
  double log_mean  = log(mean) - (log_sigma * log_sigma) / 2.0;

  double random_number = normal(log_mean, log_sigma);

  sequence_.push_back(0);
  rng_lognormal_values_.push_back(random_number);
  return std::exp(random_number);
}

/**
 * Generator a binomial random number
 *
 * @param p
 * @param n
 * @return random number
 */
double RandomNumberGenerator::binomial(double p, double n) {
  double count = 0;
  for (unsigned i = 0; i < std::ceil(n); ++i) {
    double temp = uniform();
    if (temp <= p)
      count++;
  }

  sequence_.push_back(0);
  rng_binomial_values_.push_back(count);
  return count;  // Generated Number (NOT a proportion)
}

/**
 * Generate a chi squared random number
 *
 * @param df Number of random numbers to generate
 * @return random number
 */
double RandomNumberGenerator::chi_squared(unsigned df) {
  boost::normal_distribution<> normal(0.0, 1.0);
  double                       sum = 0.0;
  for (unsigned i = 0; i < df; ++i) {
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > generator(generator_, normal);
    double                                                                   random = generator();
    sum += random * random;
  }

  sequence_.push_back(0);
  rng_chi_square_values_.push_back(sum);
  return sum;
}
/*
 * Generate a gamma random number
 *
 */
double RandomNumberGenerator::gamma(double shape) {
  // Build our Gamma Distribution Generator
  boost::gamma_distribution<>                                             distGamma(shape);
  boost::variate_generator<boost::mt19937&, boost::gamma_distribution<> > generator(generator_, distGamma);
  double                                                                  value = generator();
  sequence_.push_back(0);
  rng_gamma_values_.push_back(value);
  return value;
}

/**
 * @brief
 *
 */
using std::cout;
using std::endl;
void RandomNumberGenerator::Debug() {
  cout << "------------- DEBUG RNG -----------------" << endl;
  cout << "Sequence:" << endl;

  auto lambda = [](auto& vec, const char* name) {
    cout << "------------------- " << name << " --------------------" << endl;
    unsigned line_break = 0;
    for (auto u : vec) {
      cout << u << ", ";
      ++line_break;
      if (line_break % 50 == 0)
        cout << endl;
    }
    cout << "------------------ END ------------------" << endl;
  };

  lambda(sequence_, "sequence");
  lambda(seeds_, "seeds");
  lambda(rng_uniform_values_, "uniform");
  lambda(rng_normal_values_, "normal");
  lambda(rng_lognormal_values_, "lognormal");
  lambda(rng_binomial_values_, "binomial");
  lambda(rng_chi_square_values_, "chi_square");
  lambda(rng_gamma_values_, "gamma");
}

} /* namespace utilities */
} /* namespace niwa */
