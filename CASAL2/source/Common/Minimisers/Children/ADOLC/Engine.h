/**
 * @file Engine.h
 * @author Scott Rasmussen (scott.rasmussen@zaita.com)
 * @github https://github.com/Zaita
 * @date 17/11/2014
 * @section LICENSE
 *
 * Copyright NIWA Science ©2014 - www.niwa.co.nz
 *
 * @section DESCRIPTION
 *
 * << Add Description >>
 */
#ifdef USE_AUTODIFF
#ifdef USE_ADOLC
#ifndef MINIMISERS_ADOLC_ENGINE_H_
#define MINIMISERS_ADOLC_ENGINE_H_

// Global Headers
#include "Callback.h"

#include <vector>

// namespaces
namespace niwa {
namespace minimisers {
namespace adolc {

using std::vector;

/**
 * Class definition
 */
class Engine {
public:
  Engine();
  virtual                     ~Engine();
  Double optimise(adolc::CallBack& objective,
      vector<Double>& start_values, vector<Double>& lower_bounds,
      vector<Double>& upper_bounds, int& convergence, int& max_iterations,
      int& max_evaluations, Double gradient_tolerance, double **out_hessian,
      int untransformed_hessians, Double step_size);

private:
  // members
  Double                      convergence_;
  Double                      iterations_used_;
  Double                      evaluations_used_;
  vector<Double>              final_candidates_;
};

} /* namespace adolc */
} /* namespace minimisers */
} /* namesapce niwa */

#endif /* MINIMISERS_ADOLC_ENGINE_H_ */
#endif /* USE_ADOLC */
#endif /* USE_AUTODIFF */
