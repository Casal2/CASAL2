/**
 * @file RandomNumberGenerator.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science �2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This class is responsible for providing us with random
 * numbers generated in a variety of different ways
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef UTILITIES_RANDOMNUMBERGENERATOR_H_
#define UTILITIES_RANDOMNUMBERGENERATOR_H_

// Headers
#include <boost/random.hpp>

#include "../Utilities/Types.h"

// Namespaces
namespace niwa {
namespace utilities {

/**
 * Class definition
 */
class RandomNumberGenerator {
public:
  static RandomNumberGenerator& Instance();
  virtual ~RandomNumberGenerator();
  void Reset(unsigned new_seed = 12345u);
  void Debug();

  // Accessors
  double uniform(double min = 0.0, double max = 1.0);
  double normal(double mean = 0.0, double sigma = 1.0);
  double lognormal(double mean, double cv);  // Note the mean parameter is not the same as mu, from the lognormal distribution.
  double binomial(double p, double n);
  double chi_squared(unsigned df);
  double gamma(double shape);  // assumes scale (theta) = 1

private:
  // Methods
  RandomNumberGenerator();

  // Members
  boost::mt19937        generator_;
  std::vector<unsigned> sequence_;  // 0 for rng, 1 for seed change
  std::vector<unsigned> seeds_;
  std::vector<double>   rng_uniform_values_;
  std::vector<double>   rng_normal_values_;
  std::vector<double>   rng_lognormal_values_;
  std::vector<double>   rng_binomial_values_;
  std::vector<double>   rng_chi_square_values_;
  std::vector<double>   rng_gamma_values_;
};

} /* namespace utilities */
} /* namespace niwa */
#endif /* UTILITIES_RANDOMNUMBERGENERATOR_H_ */
