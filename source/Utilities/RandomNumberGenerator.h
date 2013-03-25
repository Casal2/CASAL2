/**
 * @file RandomNumberGenerator.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
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

#include "Utilities/Types.h"

// Namespaces
namespace isam {
namespace utilities {

using isam::utilities::Double;

/**
 * Class definition
 */
class RandomNumberGenerator {
public:
  static RandomNumberGenerator& Instance();
  virtual                       ~RandomNumberGenerator();

  // Accessors
  Double                        uniform(Double min = 0.0, Double max = 1.0);
  Double                        normal(Double mean = 0.0, Double sigma = 1.0);
  Double                        log_normal(Double mean, Double cv);
  Double                        binomial(Double p, Double n);
  Double                        chi_square(unsigned df);

private:
  // Methods
  RandomNumberGenerator();

  // Members
  boost::mt19937_64             generator_;
};

} /* namespace utilities */
} /* namespace isam */
#endif /* UTILITIES_RANDOMNUMBERGENERATOR_H_ */
