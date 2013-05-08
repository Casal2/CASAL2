/**
 * @file DESolver.h
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @version 1.0
 * @date 28/02/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * This minimiser is a differential evolutionary solver.
 * It works by creating a new set of candidate variables
 * that are taken as a step between a current candidate
 * and the best candidate
 *
 * $Date: 2008-03-04 16:33:32 +1300 (Tue, 04 Mar 2008) $
 */
#ifndef USE_AUTODIFF
#ifndef MINIMISERS_DESOLVER_H_
#define MINIMISERS_DESOLVER_H_

// Headers
#include "Minimisers/Minimiser.h"

// Namespaces
namespace isam {
namespace minimisers {

/**
 * Class Definition
 */
class DESolver : public isam::Minimiser {
public:
  // Methods
  DESolver();
  virtual                     ~DESolver();
  void                        Validate();
  void                        Execute();

private:
  // Members
  unsigned                    population_size_;
  double                      crossover_probability_;
  double                      difference_scale_;
  unsigned                    max_generations_;
  double                      tolerance_;
  string                      method_;
};

} /* namespace minimisers */
} /* namespace isam */
#endif /* MINIMISERS_DESOLVER_H_ */
#endif /* USE_AUTODIFF */
