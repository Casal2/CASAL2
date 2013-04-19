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

/**
 * Class definition
 */
class RandomNumberGenerator {
public:
  static RandomNumberGenerator& Instance();
  virtual                       ~RandomNumberGenerator();

  // Accessors
  double                        uniform(double min = 0.0, double max = 1.0);
  double                        normal(double mean = 0.0, double sigma = 1.0);
  double                        log_normal(double mean, double cv);
  double                        binomial(double p, double n);
  double                        chi_square(unsigned df);

private:
  // Methods
  RandomNumberGenerator();

  // Members
  boost::mt19937_64             generator_;
};

} /* namespace utilities */
} /* namespace isam */
#endif /* UTILITIES_RANDOMNUMBERGENERATOR_H_ */
